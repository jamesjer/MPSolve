/*
 * secular.c
 *
 *  Created on: 11/apr/2011
 *      Author: leonardo
 */

#include <mps/interface.h>
#include <mps/secular.h>
#include <mps/core.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

void
usage(mps_status *s, const char* program)
{
  /* If there is not an output stream do not print
   * the help */
  if (!s->outstr)
    return;

  fprintf(s->outstr,
      "Usage: %s [-dg] [-t type] [-n degree] [-o digits] [infile]\n"
      "\n"
      "Options:\n"
      " -d          Activate debug\n"
      " -g          Use Gemignani's approach\n"
      " -t type     Type can be 'f' for floating point\n"
      "             or 'd' for DPE\n"
      " -n degree   Degree of the polynomial associated\n"
      "             associated with the secular equation.\n"
      " -o digits   Exact digits of the roots given as output.\n",
      program);

  exit (EXIT_FAILURE);
}

int
main(int argc, char** argv)
{
  mps_secular_equation* sec;
  mps_status* s;
  int n;

  s = mps_status_new();
  s->n_threads = 1;

  /* Gemignani's approach */
  mps_boolean ga = false;

  FILE* infile;
  double tmp1, tmp2;

  /* Parse options */
  mps_opt* opt;
  mps_phase phase = float_phase;
  while ((opt = mps_getopts(&argc, &argv, "gn:dt:o:")))
    {
      switch (opt->optchar)
        {
      case 'g':
        /* Gemignani's approach. Regenerate b_i after floating
         * point cycle */
        ga = true;
        break;
      case 'o':
        s->prec_out = atoi(opt->optvalue) * LOG2_10;
        break;
      case 'n':
        if (opt->optvalue)
          n = atoi(opt->optvalue);
        else
          usage(s, argv[0]);
        break;
      case 'd':
        s->DOLOG = true;
        s->logstr = stderr;
        break;
      case 't':
        switch (opt->optvalue[0])
        {
        case 'f':
          phase = float_phase;
          break;
        case 'd':
          phase = dpe_phase;
          break;
        default:
          usage(s, argv[0]);
        }
        break;
      default:
        usage(s, argv[0]);
        break;
        }

      free(opt);
    }

  if (argc > 2)
    usage(s, argv[0]);

  /* If no file is provided use standard input */
  if (argc == 1)
    infile = stdin;
  else
    infile = fopen(argv[1], "r");

  /* Create new secular equation */
  sec = mps_secular_equation_read_from_stream(s, infile);

  /* Close the file if it's not stdin */
  if (argc == 2)
    fclose (infile);

  /* Set secular equation in user data, so it will be
   * accessible by the secular equation routines. */
  s->secular_equation = sec;

  sec->starting_case = phase;

  /* If we choose gemignani's approach follow it, otherwise
   * use standard mpsolve approach applied implicitly to the
   * secular equation. */
  if (ga)
    {
      /* Select the right algorithm */
      mps_select_algorithm(s, MPS_ALGORITHM_SECULAR_GA);

      /* Solve the secular equation */
      mps_secular_ga_mpsolve(s, phase);
    }
  else
    {
      /* Select the right algorithm */
      /* Set user polynomial with our custom functions */
      mps_status_set_degree(s, sec->n);
      mps_allocate_data(s);

      mps_select_algorithm(s, MPS_ALGORITHM_SECULAR_MPSOLVE);

      /* Solve the polynomial */
      s->goal[0] = 'a';
      mps_mpsolve(s);
    }

  /* Output the roots */
  mps_copy_roots(s);
  mps_output(s);

  /* Free used data */
  mps_secular_equation_free(sec);
  mps_status_free(s);
}