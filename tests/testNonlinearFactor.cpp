/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 *  @file  testNonlinearFactor.cpp
 *  @brief Unit tests for Non-Linear Factor, 
 *  create a non linear factor graph and a values structure for it and
 *  calculate the error for the factor.
 *  @author Christian Potthast
 **/

/*STL/C++*/
#include <iostream>

#include <CppUnitLite/TestHarness.h>

// TODO: DANGEROUS, create shared pointers
#define GTSAM_MAGIC_GAUSSIAN 2

#include <gtsam/base/Testable.h>
#include <gtsam/base/Matrix.h>
#include <gtsam/base/LieVector.h>
#include <tests/smallExample.h>
#include <tests/simulated2D.h>
#include <gtsam/linear/GaussianFactor.h>
#include <gtsam/nonlinear/NonlinearFactorGraph.h>
#include <gtsam/inference/Symbol.h>

using namespace std;
using namespace gtsam;
using namespace example;

// Convenience for named keys
using symbol_shorthand::X;
using symbol_shorthand::L;

typedef boost::shared_ptr<NonlinearFactor > shared_nlf;

/* ************************************************************************* */
TEST( NonlinearFactor, equals )
{
  SharedNoiseModel sigma(noiseModel::Isotropic::Sigma(2,1.0));

  // create two nonlinear2 factors
  Point2 z3(0.,-1.);
  simulated2D::Measurement f0(z3, sigma, X(1),L(1));

  // measurement between x2 and l1
  Point2 z4(-1.5, -1.);
  simulated2D::Measurement f1(z4, sigma, X(2),L(1));

  CHECK(assert_equal(f0,f0));
  CHECK(f0.equals(f0));
  CHECK(!f0.equals(f1));
  CHECK(!f1.equals(f0));
}

/* ************************************************************************* */
TEST( NonlinearFactor, equals2 )
{
  // create a non linear factor graph
  NonlinearFactorGraph fg = createNonlinearFactorGraph();

  // get two factors
  NonlinearFactorGraph::sharedFactor f0 = fg[0], f1 = fg[1];

  CHECK(f0->equals(*f0));
  CHECK(!f0->equals(*f1));
  CHECK(!f1->equals(*f0));
}

/* ************************************************************************* */
TEST( NonlinearFactor, NonlinearFactor )
{
  // create a non linear factor graph
  NonlinearFactorGraph fg = createNonlinearFactorGraph();

  // create a values structure for the non linear factor graph
  Values cfg = createNoisyValues();

  // get the factor "f1" from the factor graph
  NonlinearFactorGraph::sharedFactor factor = fg[0];

  // calculate the error_vector from the factor "f1"
  // error_vector = [0.1 0.1]
  Vector actual_e = boost::dynamic_pointer_cast<NoiseModelFactor>(factor)->unwhitenedError(cfg);
  CHECK(assert_equal(0.1*ones(2),actual_e));

  // error = 0.5 * [1 1] * [1;1] = 1
  double expected = 1.0;

  // calculate the error from the factor "f1"
  double actual = factor->error(cfg);
  DOUBLES_EQUAL(expected,actual,0.00000001);
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_f1 )
{
  Values c = createNoisyValues();

  // Grab a non-linear factor
  NonlinearFactorGraph nfg = createNonlinearFactorGraph();
  NonlinearFactorGraph::sharedFactor nlf = nfg[0];

  // We linearize at noisy config from SmallExample
  GaussianFactor::shared_ptr actual = nlf->linearize(c);

  GaussianFactorGraph lfg = createGaussianFactorGraph();
  GaussianFactor::shared_ptr expected = lfg[0];

  CHECK(assert_equal(*expected,*actual));

  // The error |A*dx-b| approximates (h(x0+dx)-z) = -error_vector
  // Hence i.e., b = approximates z-h(x0) = error_vector(x0)
  //CHECK(assert_equal(nlf->error_vector(c),actual->get_b()));
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_f2 )
{
  Values c = createNoisyValues();

  // Grab a non-linear factor
  NonlinearFactorGraph nfg = createNonlinearFactorGraph();
  NonlinearFactorGraph::sharedFactor nlf = nfg[1];

  // We linearize at noisy config from SmallExample
  GaussianFactor::shared_ptr actual = nlf->linearize(c);

  GaussianFactorGraph lfg = createGaussianFactorGraph();
  GaussianFactor::shared_ptr expected = lfg[1];

  CHECK(assert_equal(*expected,*actual));
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_f3 )
{
  // Grab a non-linear factor
  NonlinearFactorGraph nfg = createNonlinearFactorGraph();
  NonlinearFactorGraph::sharedFactor nlf = nfg[2];

  // We linearize at noisy config from SmallExample
  Values c = createNoisyValues();
  GaussianFactor::shared_ptr actual = nlf->linearize(c);

  GaussianFactorGraph lfg = createGaussianFactorGraph();
  GaussianFactor::shared_ptr expected = lfg[2];

  CHECK(assert_equal(*expected,*actual));
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_f4 )
{
  // Grab a non-linear factor
  NonlinearFactorGraph nfg = createNonlinearFactorGraph();
  NonlinearFactorGraph::sharedFactor nlf = nfg[3];

  // We linearize at noisy config from SmallExample
  Values c = createNoisyValues();
  GaussianFactor::shared_ptr actual = nlf->linearize(c);

  GaussianFactorGraph lfg = createGaussianFactorGraph();
  GaussianFactor::shared_ptr expected = lfg[3];

  CHECK(assert_equal(*expected,*actual));
}

/* ************************************************************************* */
TEST( NonlinearFactor, size )
{
  // create a non linear factor graph
  NonlinearFactorGraph fg = createNonlinearFactorGraph();

  // create a values structure for the non linear factor graph
  Values cfg = createNoisyValues();

  // get some factors from the graph
  NonlinearFactorGraph::sharedFactor factor1 = fg[0], factor2 = fg[1],
      factor3 = fg[2];

  CHECK(factor1->size() == 1);
  CHECK(factor2->size() == 2);
  CHECK(factor3->size() == 2);
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_constraint1 )
{
  Vector sigmas = (Vector(2) << 0.2, 0.0);
  SharedDiagonal constraint = noiseModel::Constrained::MixedSigmas(sigmas);

  Point2 mu(1., -1.);
  NonlinearFactorGraph::sharedFactor f0(new simulated2D::Prior(mu, constraint, X(1)));

  Values config;
  config.insert(X(1), Point2(1.0, 2.0));
  GaussianFactor::shared_ptr actual = f0->linearize(config);

  // create expected
  Vector b = (Vector(2) << 0., -3.);
  JacobianFactor expected(X(1), (Matrix(2, 2) << 5.0, 0.0, 0.0, 1.0), b,
    noiseModel::Constrained::MixedSigmas((Vector(2) << 1.0, 0.0)));
  CHECK(assert_equal((const GaussianFactor&)expected, *actual));
}

/* ************************************************************************* */
TEST( NonlinearFactor, linearize_constraint2 )
{
  Vector sigmas = (Vector(2) << 0.2, 0.0);
  SharedDiagonal constraint = noiseModel::Constrained::MixedSigmas(sigmas);

  Point2 z3(1.,-1.);
  simulated2D::Measurement f0(z3, constraint, X(1),L(1));

  Values config;
  config.insert(X(1), Point2(1.0, 2.0));
  config.insert(L(1), Point2(5.0, 4.0));
  GaussianFactor::shared_ptr actual = f0.linearize(config);

  // create expected
  Matrix A = (Matrix(2, 2) << 5.0, 0.0, 0.0, 1.0);
  Vector b = (Vector(2) << -15., -3.);
  JacobianFactor expected(X(1), -1*A, L(1), A, b,
    noiseModel::Constrained::MixedSigmas((Vector(2) << 1.0, 0.0)));
  CHECK(assert_equal((const GaussianFactor&)expected, *actual));
}

/* ************************************************************************* */
class TestFactor4 : public NoiseModelFactor4<LieVector, LieVector, LieVector, LieVector> {
public:
  typedef NoiseModelFactor4<LieVector, LieVector, LieVector, LieVector> Base;
  TestFactor4() : Base(noiseModel::Diagonal::Sigmas((Vector(1) << 2.0)), X(1), X(2), X(3), X(4)) {}

  virtual Vector
    evaluateError(const LieVector& x1, const LieVector& x2, const LieVector& x3, const LieVector& x4,
        boost::optional<Matrix&> H1 = boost::none,
        boost::optional<Matrix&> H2 = boost::none,
        boost::optional<Matrix&> H3 = boost::none,
        boost::optional<Matrix&> H4 = boost::none) const {
    if(H1) {
      *H1 = (Matrix(1, 1) << 1.0);
      *H2 = (Matrix(1, 1) << 2.0);
      *H3 = (Matrix(1, 1) << 3.0);
      *H4 = (Matrix(1, 1) << 4.0);
    }
    return (Vector(1) << x1 + x2 + x3 + x4).finished();
  }

  virtual gtsam::NonlinearFactor::shared_ptr clone() const {
    return boost::static_pointer_cast<gtsam::NonlinearFactor>(
        gtsam::NonlinearFactor::shared_ptr(new TestFactor4(*this))); }
};

/* ************************************ */
TEST(NonlinearFactor, NoiseModelFactor4) {
  TestFactor4 tf;
  Values tv;
  tv.insert(X(1), LieVector((Vector(1) << 1.0)));
  tv.insert(X(2), LieVector((Vector(1) << 2.0)));
  tv.insert(X(3), LieVector((Vector(1) << 3.0)));
  tv.insert(X(4), LieVector((Vector(1) << 4.0)));
  EXPECT(assert_equal((Vector(1) << 10.0), tf.unwhitenedError(tv)));
  DOUBLES_EQUAL(25.0/2.0, tf.error(tv), 1e-9);
  JacobianFactor jf(*boost::dynamic_pointer_cast<JacobianFactor>(tf.linearize(tv)));
  LONGS_EQUAL((long)X(1), (long)jf.keys()[0]);
  LONGS_EQUAL((long)X(2), (long)jf.keys()[1]);
  LONGS_EQUAL((long)X(3), (long)jf.keys()[2]);
  LONGS_EQUAL((long)X(4), (long)jf.keys()[3]);
  EXPECT(assert_equal((Matrix)(Matrix(1, 1) << 0.5), jf.getA(jf.begin())));
  EXPECT(assert_equal((Matrix)(Matrix(1, 1) << 1.0), jf.getA(jf.begin()+1)));
  EXPECT(assert_equal((Matrix)(Matrix(1, 1) << 1.5), jf.getA(jf.begin()+2)));
  EXPECT(assert_equal((Matrix)(Matrix(1, 1) << 2.0), jf.getA(jf.begin()+3)));
  EXPECT(assert_equal((Vector)(Vector(1) << -5.0), jf.getb()));
}

/* ************************************************************************* */
class TestFactor5 : public NoiseModelFactor5<LieVector, LieVector, LieVector, LieVector, LieVector> {
public:
  typedef NoiseModelFactor5<LieVector, LieVector, LieVector, LieVector, LieVector> Base;
  TestFactor5() : Base(noiseModel::Diagonal::Sigmas((Vector(1) << 2.0)), X(1), X(2), X(3), X(4), X(5)) {}

  virtual Vector
    evaluateError(const X1& x1, const X2& x2, const X3& x3, const X4& x4, const X5& x5,
        boost::optional<Matrix&> H1 = boost::none,
        boost::optional<Matrix&> H2 = boost::none,
        boost::optional<Matrix&> H3 = boost::none,
        boost::optional<Matrix&> H4 = boost::none,
        boost::optional<Matrix&> H5 = boost::none) const {
    if(H1) {
      *H1 = (Matrix(1, 1) << 1.0);
      *H2 = (Matrix(1, 1) << 2.0);
      *H3 = (Matrix(1, 1) << 3.0);
      *H4 = (Matrix(1, 1) << 4.0);
      *H5 = (Matrix(1, 1) << 5.0);
    }
    return (Vector(1) << x1 + x2 + x3 + x4 + x5).finished();
  }
};

/* ************************************ */
TEST(NonlinearFactor, NoiseModelFactor5) {
  TestFactor5 tf;
  Values tv;
  tv.insert(X(1), LieVector((Vector(1) << 1.0)));
  tv.insert(X(2), LieVector((Vector(1) << 2.0)));
  tv.insert(X(3), LieVector((Vector(1) << 3.0)));
  tv.insert(X(4), LieVector((Vector(1) << 4.0)));
  tv.insert(X(5), LieVector((Vector(1) << 5.0)));
  EXPECT(assert_equal((Vector(1) << 15.0), tf.unwhitenedError(tv)));
  DOUBLES_EQUAL(56.25/2.0, tf.error(tv), 1e-9);
  JacobianFactor jf(*boost::dynamic_pointer_cast<JacobianFactor>(tf.linearize(tv)));
  LONGS_EQUAL((long)X(1), (long)jf.keys()[0]);
  LONGS_EQUAL((long)X(2), (long)jf.keys()[1]);
  LONGS_EQUAL((long)X(3), (long)jf.keys()[2]);
  LONGS_EQUAL((long)X(4), (long)jf.keys()[3]);
  LONGS_EQUAL((long)X(5), (long)jf.keys()[4]);
  EXPECT(assert_equal((Matrix)(Matrix(1, 1) << 0.5), jf.getA(jf.begin())));
  EXPECT(assert_equal((Matrix)(Matrix(1, 1) << 1.0), jf.getA(jf.begin()+1)));
  EXPECT(assert_equal((Matrix)(Matrix(1, 1) << 1.5), jf.getA(jf.begin()+2)));
  EXPECT(assert_equal((Matrix)(Matrix(1, 1) << 2.0), jf.getA(jf.begin()+3)));
  EXPECT(assert_equal((Matrix)(Matrix(1, 1) << 2.5), jf.getA(jf.begin()+4)));
  EXPECT(assert_equal((Vector)(Vector(1) << -7.5), jf.getb()));
}

/* ************************************************************************* */
class TestFactor6 : public NoiseModelFactor6<LieVector, LieVector, LieVector, LieVector, LieVector, LieVector> {
public:
  typedef NoiseModelFactor6<LieVector, LieVector, LieVector, LieVector, LieVector, LieVector> Base;
  TestFactor6() : Base(noiseModel::Diagonal::Sigmas((Vector(1) << 2.0)), X(1), X(2), X(3), X(4), X(5), X(6)) {}

  virtual Vector
    evaluateError(const X1& x1, const X2& x2, const X3& x3, const X4& x4, const X5& x5, const X6& x6,
        boost::optional<Matrix&> H1 = boost::none,
        boost::optional<Matrix&> H2 = boost::none,
        boost::optional<Matrix&> H3 = boost::none,
        boost::optional<Matrix&> H4 = boost::none,
        boost::optional<Matrix&> H5 = boost::none,
        boost::optional<Matrix&> H6 = boost::none) const {
    if(H1) {
      *H1 = (Matrix(1, 1) << 1.0);
      *H2 = (Matrix(1, 1) << 2.0);
      *H3 = (Matrix(1, 1) << 3.0);
      *H4 = (Matrix(1, 1) << 4.0);
      *H5 = (Matrix(1, 1) << 5.0);
      *H6 = (Matrix(1, 1) << 6.0);
    }
    return (Vector(1) << x1 + x2 + x3 + x4 + x5 + x6).finished();
  }

};

/* ************************************ */
TEST(NonlinearFactor, NoiseModelFactor6) {
  TestFactor6 tf;
  Values tv;
  tv.insert(X(1), LieVector((Vector(1) << 1.0)));
  tv.insert(X(2), LieVector((Vector(1) << 2.0)));
  tv.insert(X(3), LieVector((Vector(1) << 3.0)));
  tv.insert(X(4), LieVector((Vector(1) << 4.0)));
  tv.insert(X(5), LieVector((Vector(1) << 5.0)));
  tv.insert(X(6), LieVector((Vector(1) << 6.0)));
  EXPECT(assert_equal((Vector(1) << 21.0), tf.unwhitenedError(tv)));
  DOUBLES_EQUAL(110.25/2.0, tf.error(tv), 1e-9);
  JacobianFactor jf(*boost::dynamic_pointer_cast<JacobianFactor>(tf.linearize(tv)));
  LONGS_EQUAL((long)X(1), (long)jf.keys()[0]);
  LONGS_EQUAL((long)X(2), (long)jf.keys()[1]);
  LONGS_EQUAL((long)X(3), (long)jf.keys()[2]);
  LONGS_EQUAL((long)X(4), (long)jf.keys()[3]);
  LONGS_EQUAL((long)X(5), (long)jf.keys()[4]);
  LONGS_EQUAL((long)X(6), (long)jf.keys()[5]);
  EXPECT(assert_equal((Matrix)(Matrix(1, 1) << 0.5), jf.getA(jf.begin())));
  EXPECT(assert_equal((Matrix)(Matrix(1, 1) << 1.0), jf.getA(jf.begin()+1)));
  EXPECT(assert_equal((Matrix)(Matrix(1, 1) << 1.5), jf.getA(jf.begin()+2)));
  EXPECT(assert_equal((Matrix)(Matrix(1, 1) << 2.0), jf.getA(jf.begin()+3)));
  EXPECT(assert_equal((Matrix)(Matrix(1, 1) << 2.5), jf.getA(jf.begin()+4)));
  EXPECT(assert_equal((Matrix)(Matrix(1, 1) << 3.0), jf.getA(jf.begin()+5)));
  EXPECT(assert_equal((Vector)(Vector(1) << -10.5), jf.getb()));

}

/* ************************************************************************* */
TEST( NonlinearFactor, clone_rekey )
{
  shared_nlf init(new TestFactor4());
  EXPECT_LONGS_EQUAL((long)X(1), (long)init->keys()[0]);
  EXPECT_LONGS_EQUAL((long)X(2), (long)init->keys()[1]);
  EXPECT_LONGS_EQUAL((long)X(3), (long)init->keys()[2]);
  EXPECT_LONGS_EQUAL((long)X(4), (long)init->keys()[3]);

  // Standard clone
  shared_nlf actClone = init->clone();
  EXPECT(actClone.get() != init.get()); // Ensure different pointers
  EXPECT(assert_equal(*init, *actClone));

  // Re-key factor - clones with different keys
  std::vector<Key> new_keys(4);
  new_keys[0] = X(5);
  new_keys[1] = X(6);
  new_keys[2] = X(7);
  new_keys[3] = X(8);
  shared_nlf actRekey = init->rekey(new_keys);
  EXPECT(actRekey.get() != init.get()); // Ensure different pointers

  // Ensure init is unchanged
  EXPECT_LONGS_EQUAL((long)X(1), (long)init->keys()[0]);
  EXPECT_LONGS_EQUAL((long)X(2), (long)init->keys()[1]);
  EXPECT_LONGS_EQUAL((long)X(3), (long)init->keys()[2]);
  EXPECT_LONGS_EQUAL((long)X(4), (long)init->keys()[3]);

  // Check new keys
  EXPECT_LONGS_EQUAL((long)X(5), (long)actRekey->keys()[0]);
  EXPECT_LONGS_EQUAL((long)X(6), (long)actRekey->keys()[1]);
  EXPECT_LONGS_EQUAL((long)X(7), (long)actRekey->keys()[2]);
  EXPECT_LONGS_EQUAL((long)X(8), (long)actRekey->keys()[3]);
}

/* ************************************************************************* */
int main() { TestResult tr; return TestRegistry::runAllTests(tr);}
/* ************************************************************************* */
