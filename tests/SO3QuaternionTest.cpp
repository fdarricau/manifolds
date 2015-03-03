#include <iostream>
#include <stdexcept>

#include <manifolds/defs.h>
#include <manifolds/utils.h>
#include <manifolds/pgs_assert.h>
#include <manifolds/SO3.h>
#include <manifolds/Point.h>
#include <manifolds/ExpMapQuaternion.h>

#ifndef _WIN32
#define BOOST_TEST_MODULE PGSolver 
#endif

#include <boost/test/unit_test.hpp>

using namespace pgs;

BOOST_AUTO_TEST_CASE(RotSpaceConstructor)
{
  SO3<ExpMapQuaternion> S;
  BOOST_CHECK_EQUAL(S.dim(), 3);
  BOOST_CHECK_EQUAL(S.representationDim(), 4);
  BOOST_CHECK_EQUAL(S.numberOfSubmanifolds(), 1);
}

BOOST_AUTO_TEST_CASE(SO3Identity)
{
  SO3<ExpMapQuaternion> S;
  Point x = S.getIdentity();
  Eigen::Map<Eigen::Quaterniond> xQ(x.value().data());
  BOOST_CHECK(xQ.matrix().isApprox(Eigen::Matrix3d::Identity()));
}
BOOST_AUTO_TEST_CASE(SO3Constructor)
{
  SO3<ExpMapQuaternion> S;
  Point x = S.getIdentity();
  Eigen::VectorXd v(4);
  v << 0,0,0,1;
  Point y = S.createPoint(v);
  BOOST_CHECK_EQUAL(x.value().size(), 4);
  BOOST_CHECK_EQUAL(x.value()[0], 0);
  BOOST_CHECK_EQUAL(x.value()[1], 0);
  BOOST_CHECK_EQUAL(x.value()[2], 0);
  BOOST_CHECK_EQUAL(x.value()[3], 1);
  BOOST_CHECK_EQUAL(y.value().size(), 4);
  BOOST_CHECK_EQUAL(y.value()[0], 0);
  BOOST_CHECK_EQUAL(y.value()[1], 0);
  BOOST_CHECK_EQUAL(y.value()[2], 0);
  BOOST_CHECK_EQUAL(y.value()[3], 1);
}

BOOST_AUTO_TEST_CASE(SO3Addition)
{
  SO3<ExpMapQuaternion> S;
  Eigen::Vector4d x = S.getIdentity().value();
  Eigen::Map<Eigen::Quaterniond> xQ(x.data());

  Eigen::Vector3d vy;
  vy << 0.1,0.2,0.3;
  S.plus(x, x, vy);
  S.plus(x, x, vy);
  Eigen::Matrix3d solution;
  solution <<  0.751909095300295,-0.507379423623623, 0.420949917315650, 
               0.583715086608147, 0.809160842538688,-0.067345590561841,
              -0.306446422838863, 0.296352579515415, 0.904580421269344;
  BOOST_CHECK(xQ.matrix().isApprox(solution));
}

BOOST_AUTO_TEST_CASE(SO3InvMap)
{
  SO3<ExpMapQuaternion> S;
  Eigen::Vector4d x = S.getIdentity().value();

  Eigen::Vector3d v;
  v << 0.12364,-0.2234234,0.325843516;
  S.plus(x, x, v);
  Eigen::Vector3d logX;
  S.invMap(logX, x);
  BOOST_CHECK(logX.isApprox(v));
}

BOOST_AUTO_TEST_CASE(SO3Substraction)
{
  SO3<ExpMapQuaternion> S;
  Eigen::Vector3d v( 0.268745647, -0.3544, 0.355546);
  Eigen::VectorXd q1 = S.getIdentity().value();
  Eigen::VectorXd q2(4);
  S.plus(q1, q1, v);
  S.plus(q2, q1, v);
  Eigen::Vector3d d;
  S.minus(d,q2,q1);
  BOOST_CHECK_CLOSE(d[0], v(0), 1e-8);
  BOOST_CHECK_CLOSE(d[1], v(1), 1e-8);
  BOOST_CHECK_CLOSE(d[2], v(2), 1e-8);
}

BOOST_AUTO_TEST_CASE(SO3Diff)
{
  double prec = 1e-9;
  SO3<ExpMapQuaternion> S;
  Eigen::Vector3d v(0.680375, -0.211234, 0.566198);
  Eigen::Vector4d q = S.getIdentity().value();
  S.plus(q, q, v);
  Eigen::Matrix<double, 4, 3> J;
  Eigen::Vector4d dqdvx, dqdvy, dqdvz; 
  Eigen::Vector4d qpdx, qpdy, qpdz; 
  Eigen::Vector3d dvx, dvy, dvz;
  dvx << prec, 0, 0;
  dvy << 0, prec, 0;
  dvz << 0, 0, prec;
  S.plus(qpdx,q,dvx);
  S.plus(qpdy,q,dvy);
  S.plus(qpdz,q,dvz);
  J.col(0) = (qpdx-q)/prec;
  J.col(1) = (qpdy-q)/prec;
  J.col(2) = (qpdz-q)/prec;

  Eigen::Matrix<double, 4, 3> diffM = S.diffMap(q);

  BOOST_CHECK(J.isApprox(diffM, 1e-6));
}

BOOST_AUTO_TEST_CASE(SO3ApplyDiff)
{
  int c = 5;
  SO3<ExpMapQuaternion> S;
  Index dim = S.dim();
  Index repDim = S.representationDim();
  Eigen::MatrixXd Jf = Eigen::MatrixXd::Random(c,repDim);
  Eigen::VectorXd x = S.getIdentity().value();
  S.plus(x, x, Eigen::VectorXd::Random(dim));
  Eigen::MatrixXd expectedRes;
  expectedRes = Jf*S.diffMap(x);
  Eigen::MatrixXd J(c,dim);
  S.applyDiffMap(J, Jf, x);
  BOOST_CHECK(expectedRes.isApprox(J));
}

//BOOST_AUTO_TEST_CASE(SO3invDiff)
//{
//  SO3<ExpMapMatrix> S;
//  Eigen::MatrixXd J;
//  Eigen::MatrixXd Jtest(3,9);
//  Jtest <<
//  -0.064043491813865,                 0,                  0,                  0, -0.064043491813865, 0.545030346992499,                 0, -0.545030346992499, -0.064043491813865,
//  -0.110117993664377,                 0, -0.545030346992499,                  0, -0.110117993664377,                 0, 0.545030346992499,                  0, -0.110117993664377,
//  -0.042109988599266, 0.545030346992499,                  0, -0.545030346992499, -0.042109988599266,                 0,                 0,                  0, -0.042109988599266;
//  Eigen::Vector3d v(0.3403857, 0.58526775, 0.223811);
//  Eigen::VectorXd x = S.getIdentity().value();
//  S.plus(x,x,v);
//  J = S.diffInvMap(x);
//  BOOST_CHECK(J.isApprox(Jtest));
//}
//
////BOOST_AUTO_TEST_CASE(SO3invDiffSmallValue)
////{
////  SO3<ExpMapMatrix> S;
////  Eigen::MatrixXd J;
////  Eigen::MatrixXd Jtest(3,9);
////  Jtest <<
////  -0.064043491813865,                 0,                  0,                  0, -0.064043491813865, 0.545030346992499,                 0, -0.545030346992499, -0.064043491813865,
////  -0.110117993664377,                 0, -0.545030346992499,                  0, -0.110117993664377,                 0, 0.545030346992499,                  0, -0.110117993664377,
////  -0.042109988599266, 0.545030346992499,                  0, -0.545030346992499, -0.042109988599266,                 0,                 0,                  0, -0.042109988599266;
////  Eigen::Vector3d v( 1.0e-08*0.081125768865785, 1.0e-08*0.929385970968730, 1.0e-08*0.775712678608402);
////  Point x = S.getIdentity();
////  x.increment(v);
////  J = S.diffInvMap(x.value());
////  std::cout << "J = " << J << std::endl; 
////  BOOST_CHECK(J.isApprox(Jtest));
////}
//
//BOOST_AUTO_TEST_CASE(SO3ApplyInvDiff)
//{
//  int c = 5;
//  SO3<ExpMapMatrix> S;
//  Index dim = S.dim();
//  Index repDim = S.representationDim();
//  Eigen::MatrixXd Jf = Eigen::MatrixXd::Random(c,dim);
//  Eigen::VectorXd x = S.getIdentity().value();
//  S.plus(x, x, Eigen::VectorXd::Random(dim));
//  Eigen::MatrixXd expectedRes;
//  expectedRes = Jf*S.diffInvMap(x);
//  Eigen::MatrixXd J(c,repDim);
//  S.applyDiffInvMap(J, Jf, x);
//  BOOST_CHECK(expectedRes.isApprox(J));
//}
//
//BOOST_AUTO_TEST_CASE(SO3Transport)
//{
//  int c = 4;
//  SO3<ExpMapMatrix> S;
//  Index dim = S.dim();
//  Eigen::MatrixXd H(dim,c);
//  H <<  1, 2, 3, 4,
//        5, 6, 7, 8,
//        9,10,11,12;
//  Eigen::MatrixXd Hout(dim,c);
//  Eigen::VectorXd v(dim);
//  v <<  0.083549465660115, 0.164064455761495, 0.287252050630289;
//  Eigen::VectorXd x = S.getIdentity().value();
//  S.plus(x, x, Eigen::VectorXd::Random(3));
//  Eigen::MatrixXd expectedRes(dim,c);
//  expectedRes << 1.126248257109656, 1.969921592423433, 2.813594927737210, 3.657268263050987,
//                 4.539510349826134, 5.725092676723538, 6.910675003620942, 8.096257330518345,
//                 9.226289104899047, 10.165762281434207, 11.105235457969370, 12.044708634504529;
//  S.applyTransport(Hout, H, x, v);
//  BOOST_CHECK(expectedRes.isApprox(Hout));
//}
//
//BOOST_AUTO_TEST_CASE(SO3InvTransport)
//{
//  int r = 4;
//  SO3<ExpMapMatrix> S;
//  Index dim = S.dim();
//  Eigen::MatrixXd H(r,dim);
//  H <<  1, 2, 3,
//        4, 5, 6,
//        7, 8, 9,
//        10, 11, 12;
//  Eigen::MatrixXd Hout(r,dim);
//  Eigen::VectorXd v(dim);
//  v << 0.289466560559783, 0.047283924503264, 0.291177834528185;
//  Eigen::VectorXd x = S.getIdentity().value();
//  S.plus(x, x, Eigen::VectorXd::Random(3));
//  Eigen::MatrixXd expectedRes(r,dim);
//  expectedRes <<  0.667168954696934, 1.299987987788895,  3.444548855437121,
//                  2.972337006917136, 4.096292499301232,  7.168375023495865,
//                  5.277505059137337, 6.892597010813567, 10.892201191554610,
//                  7.582673111357540, 9.688901522325903, 14.616027359613355;
//  S.applyInvTransport(Hout, H, x, v);
//  BOOST_CHECK(expectedRes.isApprox(Hout));
//}
//
//#if   EIGEN_WORLD_VERSION > 3 \
//  || (EIGEN_WORLD_VERSION == 3 && EIGEN_MAJOR_VERSION > 2) \
//  || (EIGEN_WORLD_VERSION == 3 && EIGEN_MAJOR_VERSION == 2 && EIGEN_MINOR_VERSION > 0)
//BOOST_AUTO_TEST_CASE(SO3NoAllocation)
//{
//  //We only test here that the operations on the manifold do not create
//  //temporary. Passing arguments that are not recognize by the Eigen::Ref will
//  //create temporaries, but this is the user's fault.
//  const int r = 100;
//  SO3<ExpMapMatrix> S;
//  Index dim = S.dim();
//  Index repDim = S.representationDim();
//  Eigen::VectorXd x = Eigen::VectorXd::Random(repDim);
//  Eigen::VectorXd p = Eigen::VectorXd::Random(dim);
//  Eigen::VectorXd y = Eigen::VectorXd::Random(repDim);
//  Eigen::VectorXd z(repDim);
//  Eigen::VectorXd d(dim);
//  Eigen::MatrixXd J0 = Eigen::MatrixXd::Random(r, repDim);
//  Eigen::MatrixXd J1(r, dim);
//  Eigen::MatrixXd J2(r, repDim);
//  Eigen::MatrixXd H0 = Eigen::MatrixXd::Random(S.dim(), S.dim());
//  Eigen::MatrixXd H1 = Eigen::MatrixXd::Random(S.dim(), S.dim());
//  Eigen::MatrixXd H2 = Eigen::MatrixXd::Random(S.dim(), S.dim());
//
//  //The first call to the following methods might trigger a memory allocation
//  //depending on the size of the Ji and the initial buffer size inside S.
//  //However, subsequent calls should not require any allocation, what we check
//  //after.
//  S.applyDiffMap(J1, J0, x);
//  S.applyDiffInvMap(J2, J1, x);
//
//  Eigen::internal::set_is_malloc_allowed(false);
//  utils::set_is_malloc_allowed(false);
//  {
//    std::cout << "Memory allocation tests:" << std::endl;
//    S.plus(z, x, p);
//    std::cout << "- method 'plus' passed" << std::endl;
//    S.minus(d, x, y);
//    std::cout << "- method 'minus' passed" << std::endl;
//    S.invMap(d, x);
//    std::cout << "- method 'invMap' passed" << std::endl;
//    S.applyDiffMap(J1, J0, x);
//    std::cout << "- method 'applyDiffMap' passed" << std::endl;
//    S.applyDiffInvMap(J2, J1, x);
//    std::cout << "- method 'applyDiffInvMap' passed" << std::endl;
//    S.applyTransport(H1, H0, x, p);
//    std::cout << "- method 'applyTransport' passed" << std::endl;
//    S.applyInvTransport(H2, H0, x, p);
//    std::cout << "- method 'applyInvTransport' passed" << std::endl;
//  }
//  utils::set_is_malloc_allowed(true);
//  Eigen::internal::set_is_malloc_allowed(true);
//}
//#endif
