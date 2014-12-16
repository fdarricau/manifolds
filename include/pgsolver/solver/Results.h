#ifndef _PGS_RESULTS_H_
#define _PGS_RESULTS_H_

#include <Eigen/Core>
#include <pgsolver/manifolds/Point.h>

namespace pgs
{
  enum eStatus
  {
    CONVERGE,                       // solver has converge properly
    INFEASIBLE_PROBLEM,             // the problem is infeasible (due to nonlinear constraints)
    INFEASIBLE_LINEAR_CONSTRAINTS,  // the problem is infeasible 
    INFEASIBLE_BOUNDS,
    INFEASIBLE_QP,
    MAX_ITER,
    ERROR
  };

  struct LagrangeMultipliers
  {

  };

  struct Results
  {
    Point               x_star;
    eStatus             status;
    LagrangeMultipliers lambda;
  };
}

#endif //_PGS_RESULTS_H_