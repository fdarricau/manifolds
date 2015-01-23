#ifndef _PGS_HESSIAN_UPDATER_H_
#define _PGS_HESSIAN_UPDATER_H_

#include <Eigen/Core>

#include <pgsolver/manifolds/Point.h>

#include <pgsolver/solver/SolverOptions.h>

namespace pgs
{
  class HessianUpdater
  {
    public:
      static void hessianUpdate(Eigen::MatrixXd& H, const Point& x, const double alpha, 
          const Eigen::VectorXd& step, const Eigen::MatrixXd& prevDiffLag, 
          const Eigen::MatrixXd& diffLag,
          const SolverOptions& solverOptions);
      /// \brief Performs a BFGS update on B
      static void computeBFGS(Eigen::MatrixXd& B, const Eigen::VectorXd& s,const Eigen::VectorXd& y);
      /// \brief Performs a SR1 update on B
      static void computeSR1(Eigen::MatrixXd& B, const Eigen::VectorXd& s,const Eigen::VectorXd& y);
  };
}

#endif //_PGS_HESSIAN_UPDATER_H_
