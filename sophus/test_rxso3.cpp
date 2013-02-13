// This file is part of Sophus.
//
// Copyright 2012-2013 Hauke Strasdat
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights  to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include <iostream>
#include <vector>

#include <unsupported/Eigen/MatrixFunctions>

#include "rxso3.hpp"

using namespace Sophus;
using namespace std;

template<class Scalar>
bool rxso3explog_tests() {
  typedef RxSO3Group<Scalar> RxSO3Type;
  typedef typename RxSO3Group<Scalar>::TangentType TangentType;
  typedef typename RxSO3Group<Scalar>::AdjointType AdjointType;
  typedef typename RxSO3Group<Scalar>::PointType  PointType;
  typedef typename RxSO3Group<Scalar>::TransformationType  TransformationType;
  const Scalar SMALL_EPS = SophusConstants<Scalar>::epsilon();
  const Scalar PI = SophusConstants<Scalar>::pi();

  vector<RxSO3Type> rxso3_vec;
  rxso3_vec.push_back(RxSO3Type::exp(TangentType(0.2, 0.5, 0.0, 1.)));
  rxso3_vec.push_back(RxSO3Type::exp(TangentType(0.2, 0.5, -1.0, 1.1)));
  rxso3_vec.push_back(RxSO3Type::exp(TangentType(0., 0., 0., 1.1)));
  rxso3_vec.push_back(RxSO3Type::exp(TangentType(0., 0., 0.00001, 0.)));
  rxso3_vec.push_back(RxSO3Type::exp(TangentType(0., 0., 0.00001, 0.00001)));
  rxso3_vec.push_back(RxSO3Type::exp(TangentType(0., 0., 0.00001, 0)));
  rxso3_vec.push_back(RxSO3Type::exp(TangentType(PI, 0, 0, 0.9)));
  rxso3_vec.push_back(RxSO3Type::exp(TangentType(0.2, 0.5, 0.0,0))
                   *RxSO3Type::exp(TangentType(PI, 0, 0,0.0))
                   *RxSO3Type::exp(TangentType(-0.2, -0.5, -0.0,0)));
  rxso3_vec.push_back(RxSO3Type::exp(TangentType(0.3, 0.5, 0.1,0))
                   *RxSO3Type::exp(TangentType(PI, 0, 0,0))
                   *RxSO3Type::exp(TangentType(-0.3, -0.5, -0.1,0)));

  bool failed = false;

  for (size_t i=0; i<rxso3_vec.size(); ++i) {
    TransformationType sR1 = rxso3_vec[i].matrix();
    TransformationType sR2 = RxSO3Type::exp(rxso3_vec[i].log()).matrix();
    TransformationType DiffR = sR1-sR2;
    Scalar nrm = DiffR.norm();

    //// ToDO: Force RxSO3Type to be more accurate!
    if (isnan(nrm) || nrm>SMALL_EPS) {
      cerr << "RxSO3Type - exp(log(RxSO3Type))" << endl;
      cerr  << "Test case: " << i << endl;
      cerr << sR1 << endl;
      cerr << rxso3_vec[i].log() << endl;
      cerr << sR2 << endl;
      cerr << DiffR <<endl;
      cerr << endl;
      failed = true;
    }

  }

  for (size_t i=0; i<rxso3_vec.size(); ++i) {
    PointType p(1,2,4);
    TransformationType sR = rxso3_vec[i].matrix();
    PointType res1 = rxso3_vec[i]*p;
    PointType res2 = sR*p;

    Scalar nrm = (res1-res2).norm();

    if (isnan(nrm) || nrm>SMALL_EPS) {
      cerr << "Transform vector" << endl;
      cerr  << "Test case: " << i << endl;
      cerr << (res1-res2) <<endl;
      cerr << endl;
      failed = true;
    }
  }

  for (size_t i=0; i<rxso3_vec.size(); ++i) {
    TransformationType q = rxso3_vec[i].matrix();
    TransformationType inv_q = rxso3_vec[i].inverse().matrix();
    TransformationType res = q*inv_q ;
    TransformationType I;
    I.setIdentity();

    Scalar nrm = (res-I).norm();

    if (isnan(nrm) || nrm>SMALL_EPS) {
      cerr << "Inverse" << endl;
      cerr  << "Test case: " << i << endl;
      cerr << (res-I) <<endl;
      cerr << endl;
      failed = true;
    }
  }
  for (size_t i=0; i<rxso3_vec.size(); ++i) {
    TransformationType T = rxso3_vec[i].matrix();
    AdjointType Ad = rxso3_vec[i].Adj();
    TangentType x;
    x << 0.9,2,3,1.2;
    TransformationType I;
    I.setIdentity();
    TangentType ad1 = Ad*x;
    TangentType ad2 = RxSO3Type::vee(T*RxSO3Type::hat(x)
                                     *rxso3_vec[i].inverse().matrix());
    Scalar nrm = (ad1-ad2).norm();

    if (isnan(nrm) || nrm>SMALL_EPS) {
      cerr << "Adjoint" << endl;
      cerr  << "Test case: " << i << endl;
      cerr << (ad1-ad2).transpose() <<endl;
      cerr << endl;
      failed = true;
    }
  }
  for (size_t i=0; i<rxso3_vec.size(); ++i) {
    for (size_t j=0; j<rxso3_vec.size(); ++j) {
      TransformationType mul_resmat = (rxso3_vec[i]*rxso3_vec[j]).matrix();
      Scalar mul_res_raw[RxSO3Type::num_parameters];
      Eigen::Map<RxSO3Type> mul_res(mul_res_raw);
      mul_res = rxso3_vec[i];
      mul_res *= rxso3_vec[j];
      TransformationType diff =  mul_resmat-mul_res.matrix();
      Scalar nrm = diff.norm();
      if (isnan(nrm) || nrm>SMALL_EPS) {
        cerr << "Multiply and Map" << endl;
        cerr  << "Test case: " << i  << "," << j << endl;
        cerr << diff <<endl;
        cerr << endl;
        failed = true;
      }
    }
  }
  return failed;
}


template<class Scalar>
bool rxso3bracket_tests() {
  typedef RxSO3Group<Scalar> RxSO3Type;
  typedef typename RxSO3Group<Scalar>::TangentType TangentType;
  typedef typename RxSO3Group<Scalar>::TransformationType  TransformationType;
  const Scalar SMALL_EPS = SophusConstants<Scalar>::epsilon();

  bool failed = false;
  vector<TangentType> vecs;
  TangentType tmp;
  tmp << 0,0,0,0;
  vecs.push_back(tmp);
  tmp << 1,0,0,0;
  vecs.push_back(tmp);
  tmp << 1,0,0,0.1;
  vecs.push_back(tmp);
  tmp << 0,1,0,0.1;
  vecs.push_back(tmp);
  tmp << 0,0,1,-0.1;
  vecs.push_back(tmp);
  tmp << -1,1,0,-0.1;
  vecs.push_back(tmp);
  tmp << 20,-1,0,2;
  vecs.push_back(tmp);
  for (size_t i=0; i<vecs.size(); ++i) {
    TangentType resDiff = vecs[i] - RxSO3Type::vee(RxSO3Type::hat(vecs[i]));
    if (resDiff.norm()>SMALL_EPS) {
      cerr << "Hat-vee Test" << endl;
      cerr  << "Test case: " << i <<  endl;
      cerr << resDiff.transpose() << endl;
      cerr << endl;
    }

    for (size_t j=0; j<vecs.size(); ++j) {
      TangentType res1 = RxSO3Type::lieBracket(vecs[i],vecs[j]);
      TransformationType hati = RxSO3Type::hat(vecs[i]);
      TransformationType hatj = RxSO3Type::hat(vecs[j]);

      TangentType res2 = RxSO3Type::vee(hati*hatj-hatj*hati);
      TangentType resDiff = res1-res2;
      if (resDiff.norm()>SMALL_EPS) {
        cerr << "RxSO3Type Lie Bracket Test" << endl;
        cerr  << "Test case: " << i << ", " <<j<< endl;
        cerr << vecs[i].transpose() << endl;
        cerr << vecs[j].transpose() << endl;
        cerr << resDiff.transpose() << endl;
        cerr << endl;
        failed = true;
      }
    }

    TangentType omega = vecs[i];
    TransformationType exp_x = RxSO3Type::exp(omega).matrix();
    TransformationType expmap_hat_x = (RxSO3Type::hat(omega)).exp();
    TransformationType DiffR = exp_x-expmap_hat_x;
    Scalar nrm = DiffR.norm();

    if (isnan(nrm) || nrm>SMALL_EPS) {
      cerr << "expmap(hat(x)) - exp(x)" << endl;
      cerr  << "Test case: " << i << endl;
      cerr << exp_x <<endl;
      cerr << expmap_hat_x <<endl;
      cerr << DiffR <<endl;
      cerr << endl;
      failed = true;
    }
  }
  return failed;
}



int main() {
  cerr << "Test RxSO3" << endl << endl;
  cerr << "Double tests: " << endl;
  bool failed = rxso3explog_tests<double>();
  failed = failed || rxso3bracket_tests<double>();
  if (failed) {
    cerr << "failed!" << endl << endl;
    exit(-1);
  } else {
    cerr << "passed." << endl << endl;
  }

  cerr << "Float tests: " << endl;
  failed = failed || rxso3explog_tests<float>();
  failed = failed || rxso3bracket_tests<float>();
  if (failed) {
    cerr << "failed!" << endl << endl;
    exit(-1);
  } else {
    cerr << "passed." << endl << endl;
  }
  return 0;
}
