#include <iostream>
#include <boost/math/special_functions/sinc.hpp>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <manifolds/defs.h>
#include <manifolds/ExpMapQuaternion.h>
#include <manifolds/pgs_assert.h>

namespace pgs
{
  typedef Eigen::Map< Eigen::Quaterniond > toQuat;
  typedef Eigen::Map< const Eigen::Quaterniond > toConstQuat;
  const double ExpMapQuaternion::prec = 1e-8; //TODO Should be sqrt(sqrt(machine precision))

  void ExpMapQuaternion::plus_(RefVec out, const ConstRefVec& x, const ConstRefVec& v)
  {
    OutputType q;
    exponential(q,v);
    toQuat(out.data()) = (toConstQuat(x.data()))*(toConstQuat(q.data())); //out = x*exp(v)
  }

  void ExpMapQuaternion::exponential(OutputType& q, const ConstRefVec& v)
  {
    pgs_assert(v.size() == 3 && "Increment for expMap must be of size 3");
    double n2 = v.squaredNorm(); // (theta)^2 (in Grassia)
    pgs_assert(sqrt(n2) < M_PI && "Increment for expMap must be of norm at most pi");
    double s; // sin(theta/2)/theta in Grassia
    if (n2 < prec)
    {
      toQuat(q.data()).w() = 1 + (-1 + n2 / 48)*(n2/8);// cos(theta/2) in Grassia
      s = (1+(-1+0.0125*n2)*n2/24)/2;
    }
    else
    {
      double t = sqrt(n2); // theta (in Grassia)
      toQuat(q.data()).w() = cos(0.5*t);
      s = sin(0.5*t) / t;
    }
    toQuat(q.data()).vec() = s*v;
  }

  void ExpMapQuaternion::minus_(RefVec out, const ConstRefVec& x, const ConstRefVec& y)
  {
    Eigen::Vector4d tmp;
    toQuat q(tmp.data());
    const toConstQuat xQ(x.data());
    const toConstQuat yQ(y.data());
    q = yQ.inverse()*xQ; //TODO double-check that formula
    logarithm(out,tmp);
  }

  void ExpMapQuaternion::invMap_(RefVec out, const ConstRefVec& x)
  {
    logarithm(out,x);
  }

  void ExpMapQuaternion::logarithm(RefVec out, const OutputType& v)
  {
    const toConstQuat vQ(v.data());
    double n2 = vQ.vec().squaredNorm();
    double n = sqrt(n2);

    if (n < prec)
      out = (2/vQ.w())*vQ.vec();
    else
      out = atan2(2 * n * vQ.w(), vQ.w() * vQ.w() - n2) / n * vQ.vec(); 
  }

  void ExpMapQuaternion::setIdentity_(RefVec out)
  {
    out << 0,0,0,1;
  }

  bool ExpMapQuaternion::isValidInit_(const Eigen::VectorXd& val)
  {
    typedef Eigen::Map<const Eigen::Quaterniond> toQuat;
    bool out(val.size()==4);
    double norm = toQuat(val.data()).norm();
    out = out && (fabs(norm - 1) < prec);
    return out;
  }

  Eigen::Matrix<double, 4, 3> ExpMapQuaternion::diffMap_(const ConstRefVec& x)
  {
    const Eigen::Map<const Eigen::Quaterniond> xQ(x.data());
    Eigen::Matrix<double, 4, 3> J;
    J <<  0.5*xQ.w(), -0.5*xQ.z(),  0.5*xQ.y(),
          0.5*xQ.z(),  0.5*xQ.w(), -0.5*xQ.x(),
         -0.5*xQ.y(),  0.5*xQ.x(),  0.5*xQ.w(),
         -0.5*xQ.x(), -0.5*xQ.y(), -0.5*xQ.z();
    return J;
  }

  void ExpMapQuaternion::applyDiffMap_(
      RefMat out, const ConstRefMat& in, const ConstRefVec& x, ReusableTemporaryMap& m)
  {
    pgs_assert(in.cols() == OutputDim_ && "Dimensions mismatch" );
    Eigen::Map<Eigen::MatrixXd, Eigen::Aligned> a = m.getMap(in.rows(),3);
    a.noalias() = in*diffMap_(x);
    out = a;
  }

  Eigen::Matrix<double, 3, 4> ExpMapQuaternion::diffInvMap_(const ConstRefVec& )
  {
    Eigen::Matrix<double, 3, 4> J;
    J.setZero();
    return J;
  }

  void ExpMapQuaternion::applyDiffInvMap_(
      RefMat out, const ConstRefMat& in, const ConstRefVec& x, ReusableTemporaryMap& m)
  {
    pgs_assert(in.cols() == InputDim_ && "Dimensions mismatch" );
    Eigen::Map<Eigen::MatrixXd, Eigen::Aligned> a = m.getMap(in.rows(),OutputDim_);
    a.noalias() = in*diffInvMap_(x);
    out = a;
  }

  void ExpMapQuaternion::applyTransport_(RefMat out, const ConstRefMat& in, const ConstRefVec&, const ConstRefVec& v, ReusableTemporaryMap& m)
  {
    OutputType E;
    exponential(E,v);
    Eigen::Map<Eigen::MatrixXd, Eigen::Aligned> a = m.getMap(InputDim_, in.cols());
    a.noalias() = E*in;
    out = a;
  }

  void ExpMapQuaternion::applyInvTransport_(RefMat out, const ConstRefMat& in, const ConstRefVec&, const ConstRefVec& v, ReusableTemporaryMap& m)
  {
    OutputType E;
    exponential(E,v);
    Eigen::Map<Eigen::MatrixXd, Eigen::Aligned> a = m.getMap(in.rows(), InputDim_);
    a.noalias() = in*(E.transpose());
    out = a;
  }

  void ExpMapQuaternion::tangentConstraint_(RefMat, const ConstRefVec&)
  {
    //out is 0xt, no need to fill it
  }

  bool ExpMapQuaternion::isInTxM_(const ConstRefVec&, const ConstRefVec&)
  {
    return true;
  }

  void ExpMapQuaternion::forceOnTxM_(RefVec out, const ConstRefVec& in, const ConstRefVec&)
  {
    out = in;
  }

}

