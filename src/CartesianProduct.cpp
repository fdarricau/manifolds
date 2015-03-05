#include <manifolds/CartesianProduct.h>
#include <manifolds/pgs_assert.h>

namespace pgs
{
  CartesianProduct::CartesianProduct()
    : Manifold(0,0,0)
  {
    startIndexT_.push_back(0);
    startIndexR_.push_back(0);
  }

  CartesianProduct::CartesianProduct(const Manifold& m1, const Manifold& m2)
    : Manifold(0,0,0)
  {
    startIndexT_.push_back(0);
    startIndexR_.push_back(0);
    multiply(m1);
    multiply(m2);
  }

  bool CartesianProduct::isValidInit_(const Eigen::VectorXd& val) const
  {
    bool out = true;
    for (std::size_t i = 0; i<numberOfSubmanifolds(); ++i)
    {
      out = out && submanifolds_[i]->isValidInit(getConstView<R>(val, i));
    }
    return out;
  }

  CartesianProduct& CartesianProduct::multiply(const Manifold& m)
  {
    m.lock();
    setDimension(dim() + m.dim());
    setTangentDimension(tangentDim() + m.tangentDim());
    setRepresentationDimension(representationDim() + m.representationDim());
    submanifolds_.push_back(&m);
    startIndexT_.push_back(startIndexT_.back() + m.tangentDim());
    startIndexR_.push_back(startIndexR_.back() + m.representationDim());
    return *this;
  }

  size_t CartesianProduct::numberOfSubmanifolds() const
  {
    return submanifolds_.size();
  }

  const Manifold& CartesianProduct::operator()(size_t i) const
  {
    pgs_assert(i < submanifolds_.size() && "invalid index");
    return *submanifolds_[i];
  }

  std::string CartesianProduct::toString(const ConstRefVec& val, const std::string& prefix) const
  {
    std::stringstream ss;
    size_t n = numberOfSubmanifolds();
    for (std::size_t i = 0; i<n-1; ++i)
    {
      ss << submanifolds_[i]->toString(getConstView<R>(val, i), prefix + "  ") << std::endl;;
    }
    ss << submanifolds_.back()->toString(getConstView<R>(val, n - 1), prefix + "  ");
    return ss.str();
  }

  void CartesianProduct::plus_(RefVec out, const ConstRefVec& x, const ConstRefVec& v) const
  {
    for (size_t i = 0; i < submanifolds_.size(); ++i)
    {
      submanifolds_[i]->plus(getView<R>(out, i),
                              getConstView<R>(x, i),
                              getConstView<T>(v, i));
    }
  }

  void CartesianProduct::minus_(RefVec out, const ConstRefVec& x, const ConstRefVec& y) const
  {
    for (size_t i = 0; i < submanifolds_.size(); ++i)
    {
      submanifolds_[i]->minus(getView<T>(out,i),
                              getConstView<R>(x, i),
                              getConstView<R>(y, i));
    }
  }

  void CartesianProduct::invMap_(RefVec out, const ConstRefVec& x) const
  {
    for (size_t i = 0; i < submanifolds_.size(); ++i)
    {
      submanifolds_[i]->invMap(getView<T>(out,i),
                               getConstView<R>(x, i));
    }
  }

  void CartesianProduct::setZero_(RefVec out) const
  {
    for (size_t i = 0; i < submanifolds_.size(); ++i)
      submanifolds_[i]->setZero(getView<R>(out, i));
  }

  Eigen::MatrixXd CartesianProduct::diffMap_(const ConstRefVec& x ) const
  {
    Eigen::MatrixXd J(representationDim(),tangentDim());
    J.setZero();
    for (size_t i = 0; i < submanifolds_.size(); ++i)
    {
      getView<R, T>(J, i) = submanifolds_[i]->diffMap(getConstView<R>(x, i));
    }
    return J;
  }

  void CartesianProduct::applyDiffMap_(RefMat out, const ConstRefMat& in, const ConstRefVec& x) const
  {
    for (size_t i = 0; i < submanifolds_.size(); ++i)
    {
      submanifolds_[i]->applyDiffMap( getView<F, T>(out, i),
                                      getConstView<F, R>(in, i),
                                      getConstView<R>(x, i));
    }
  }

  Eigen::MatrixXd CartesianProduct::diffInvMap_(const ConstRefVec& x) const
  {
    Eigen::MatrixXd J(tangentDim(),representationDim());
    J.setZero();
    for (size_t i = 0; i < submanifolds_.size(); ++i)
    {
      getView<T, R>(J, i) = submanifolds_[i]->diffInvMap(getConstView<R>(x, i));
    }
    return J;
  }

  void CartesianProduct::applyDiffInvMap_(RefMat out, const ConstRefMat& in, const ConstRefVec& x) const
  {
    for (size_t i = 0; i < submanifolds_.size(); ++i)
    {
      submanifolds_[i]->applyDiffInvMap(getView<F,R>(out, i),
                                        getConstView<F, T>(in, i),
                                        getConstView<R>(x, i));
    }
  }

  void CartesianProduct::applyTransport_(RefMat out, const ConstRefMat& in, const ConstRefVec& x, const ConstRefVec& v) const
  {
    for (size_t i = 0; i < submanifolds_.size(); ++i)
    {
      submanifolds_[i]->applyTransport(getView<T, F>(out, i),
                                       getConstView<T, F>(in, i),
                                       getConstView<R>(x, i),
                                       getConstView<T>(v, i));
    }
  }

  void CartesianProduct::applyInvTransport_(RefMat out, const ConstRefMat& in, const ConstRefVec& x, const ConstRefVec& v) const
  {
    for (size_t i = 0; i < submanifolds_.size(); ++i)
    {
      submanifolds_[i]->applyInvTransport(getView<F, T>(out, i),
                                          getConstView<F, T>(in, i),
                                          getConstView<R>(x, i),
                                          getConstView<T>(v, i));
    }
  }

  void CartesianProduct::tangentConstraint_(RefMat out, const ConstRefVec& x) const
  {
    Index k = 0;
    out.setZero();
    for (size_t i = 0; i < submanifolds_.size(); ++i)
    {
      Index s = submanifolds_[i]->tangentDim() - submanifolds_[i]->dim();
      auto Ci = getView<F, T>(out, i);
      submanifolds_[i]->tangentConstraint(Ci.middleRows(k,s),
                                          getConstView<R>(x, i));
      k += s;
    }
  }

  bool CartesianProduct::isInTxM_(const ConstRefVec& x, const ConstRefVec& v) const
  {
    bool b = true;
    for (size_t i = 0; i < submanifolds_.size() && b; ++i)
      b = submanifolds_[i]->isInTxM(getConstView<R>(x, i), getConstView<T>(v, i));
    return b;
  }

  void CartesianProduct::forceOnTxM_(RefVec out, const ConstRefVec& in, const ConstRefVec& x) const
  {
    for (size_t i = 0; i < submanifolds_.size(); ++i)
      submanifolds_[i]->forceOnTxM(getView<T>(out, i), 
                                   getConstView<T>(in, i),
                                   getConstView<R>(x, i));
  }
}

