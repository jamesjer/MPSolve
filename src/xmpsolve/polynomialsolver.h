#ifndef POLYNOMIALSOLVER_H
#define POLYNOMIALSOLVER_H

#include <QObject>
#include "root.h"
#include "mpsolveworker.h"
#include "polynomial.h"
#include <mps/mps.h>

namespace xmpsolve {

/**
 * @brief The PolynomialSolver class aims to solve a polynomial given
 * its coefficients or the content of the line edit to parse.
 *
 * When the polynomial will be solve a solved() signal will be emitted
 * and the roots will be reachable through the get_roots() method.
 */
class PolynomialSolver : public QObject
{
    Q_OBJECT
public:
    explicit PolynomialSolver(QObject *parent = 0);
    ~PolynomialSolver();

    /**
      * @brief Solve a polynomial.
      *
      * @return The degree of the polynomial.
      */
    int solvePoly(Polynomial poly, mps_algorithm selected_algorithm = MPS_ALGORITHM_SECULAR_GA);

    /** @brief Parse the string describing the polynomial
      * and solve it.
      *
      * @return The degree of the polynomial.
      */
    int solvePoly(QString inputString, mps_algorithm = MPS_ALGORITHM_SECULAR_GA);

    /**
     * @brief errorMessage can be used to access the last error message,
     * if solvePoly() returns -1.
     * @return A QString describing the last error.
     */
    QString errorMessage();

    /**
     * @brief CPUTime can be used to access the CPU time consumed by the last polynomial
     * solved. Calling it before solvePoly() leads to undefined behaviour.
     * @return the number of ms spent on the last polynomial solution.
     */
    unsigned long int CPUTime();

private:
    MPSolveWorker m_worker;
    mps_context * m_mpsContext;
    QString m_errorMessage;
    Polynomial m_currentPoly;
    
signals:
    /** @brief Signal emitted when the computation ends. */
    void solved(QList<Root*>);
    
public slots:
    /** @brief Called when the thread solving the polynomial exits. */
    void workerExited();
    
};

} // Namespace xmpsolve

#endif // POLYNOMIALSOLVER_H