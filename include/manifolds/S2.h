#ifndef _MANIFOLDS_S2_H_
#define _MANIFOLDS_S2_H_

#include <manifolds/defs.h>
#include <manifolds/Manifold.h>

namespace pgs
{
  /// \brief Manifold representing the 3-dimensional Sphere, also
  /// known as S2.
  class S2: public Manifold
  {
  public:
    S2();
    S2(double magnitude);
    S2(const ConstRefVec& magnitude);

    virtual size_t numberOfSubmanifolds() const;
    virtual const Manifold& operator()(size_t i) const;

    virtual std::string toString(const ConstRefVec& val, const std::string& prefix = "") const;
    virtual void getTypicalMagnitude_(RefVec out) const;
    void setTypicalMagnitude(double magnitude);
    void setTypicalMagnitude(const ConstRefVec& out);
    void logarithm (RefVec out, const ConstRefVec& x, const ConstRefVec& y) const;
    double distance (const ConstRefVec& x, const ConstRefVec& y) const;
    void proj (RefMat out, const ConstRefVec& x, const ConstRefMat& y) const;

  protected:
    //map operations
    virtual bool isInM_(const Eigen::VectorXd& val) const;
    virtual void retractation_(RefVec out, const ConstRefVec& x, const ConstRefVec& v) const;
    virtual void pseudoLog_(RefVec out, const ConstRefVec& x, const ConstRefVec& y) const;
    virtual void pseudoLog0_(RefVec out, const ConstRefVec& x) const;
    virtual void setZero_(RefVec out) const;
    virtual Eigen::MatrixXd diffRetractation_(const ConstRefVec& x) const;
    virtual void applyDiffRetractation_(RefMat out, const ConstRefMat& in, const ConstRefVec& x) const;
    virtual Eigen::MatrixXd diffPseudoLog0_(const ConstRefVec& x) const;
    virtual void applyDiffPseudoLog0_(RefMat out, const ConstRefMat& in, const ConstRefVec& x) const;
    virtual void applyTransport_(RefMat out, const ConstRefMat& in, const ConstRefVec& x, const ConstRefVec& v) const;
    virtual void applyInvTransport_(RefMat out, const ConstRefMat& in, const ConstRefVec& x, const ConstRefVec& v) const;

    virtual void tangentConstraint_(RefMat out, const ConstRefVec& x) const;
    virtual bool isInTxM_(const ConstRefVec& x, const ConstRefVec& v) const;
    virtual void forceOnTxM_(RefVec out, const ConstRefVec& in, const ConstRefVec& x) const;
    virtual void limitMap_(RefVec out) const;

  private:
    Eigen::Vector3d typicalMagnitude_;
  };
}
#endif //_MANIFOLDS_S2_H_