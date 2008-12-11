// Copyright 2008, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Google Mock - a framework for writing C++ mock classes.
//
// This file tests the built-in matchers generated by a script.

#include <gmock/gmock-generated-matchers.h>

#include <list>
#include <sstream>
#include <string>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

using std::list;
using std::stringstream;
using std::vector;
using testing::_;
using testing::ElementsAre;
using testing::ElementsAreArray;
using testing::Eq;
using testing::Ge;
using testing::Gt;
using testing::MakeMatcher;
using testing::Matcher;
using testing::MatcherInterface;
using testing::Ne;
using testing::Not;
using testing::Pointee;
using testing::Ref;
using testing::StrEq;
using testing::internal::string;

// Returns the description of the given matcher.
template <typename T>
string Describe(const Matcher<T>& m) {
  stringstream ss;
  m.DescribeTo(&ss);
  return ss.str();
}

// Returns the description of the negation of the given matcher.
template <typename T>
string DescribeNegation(const Matcher<T>& m) {
  stringstream ss;
  m.DescribeNegationTo(&ss);
  return ss.str();
}

// Returns the reason why x matches, or doesn't match, m.
template <typename MatcherType, typename Value>
string Explain(const MatcherType& m, const Value& x) {
  stringstream ss;
  m.ExplainMatchResultTo(x, &ss);
  return ss.str();
}

// For testing ExplainMatchResultTo().
class GreaterThanMatcher : public MatcherInterface<int> {
 public:
  explicit GreaterThanMatcher(int rhs) : rhs_(rhs) {}

  virtual bool Matches(int lhs) const { return lhs > rhs_; }

  virtual void DescribeTo(::std::ostream* os) const {
    *os << "is greater than " << rhs_;
  }

  virtual void ExplainMatchResultTo(int lhs, ::std::ostream* os) const {
    const int diff = lhs - rhs_;
    if (diff > 0) {
      *os << "is " << diff << " more than " << rhs_;
    } else if (diff == 0) {
      *os << "is the same as " << rhs_;
    } else {
      *os << "is " << -diff << " less than " << rhs_;
    }
  }
 private:
  const int rhs_;
};

Matcher<int> GreaterThan(int n) {
  return MakeMatcher(new GreaterThanMatcher(n));
}

// Tests for ElementsAre().

// Evaluates to the number of elements in 'array'.
#define GMOCK_ARRAY_SIZE_(array) (sizeof(array)/sizeof(array[0]))

TEST(ElementsAreTest, CanDescribeExpectingNoElement) {
  Matcher<const vector<int>&> m = ElementsAre();
  EXPECT_EQ("is empty", Describe(m));
}

TEST(ElementsAreTest, CanDescribeExpectingOneElement) {
  Matcher<vector<int> > m = ElementsAre(Gt(5));
  EXPECT_EQ("has 1 element that is greater than 5", Describe(m));
}

TEST(ElementsAreTest, CanDescribeExpectingManyElements) {
  Matcher<list<string> > m = ElementsAre(StrEq("one"), "two");
  EXPECT_EQ("has 2 elements where\n"
            "element 0 is equal to \"one\",\n"
            "element 1 is equal to \"two\"", Describe(m));
}

TEST(ElementsAreTest, CanDescribeNegationOfExpectingNoElement) {
  Matcher<vector<int> > m = ElementsAre();
  EXPECT_EQ("is not empty", DescribeNegation(m));
}

TEST(ElementsAreTest, CanDescribeNegationOfExpectingOneElment) {
  Matcher<const list<int>& > m = ElementsAre(Gt(5));
  EXPECT_EQ("does not have 1 element, or\n"
            "element 0 is not greater than 5", DescribeNegation(m));
}

TEST(ElementsAreTest, CanDescribeNegationOfExpectingManyElements) {
  Matcher<const list<string>& > m = ElementsAre("one", "two");
  EXPECT_EQ("does not have 2 elements, or\n"
            "element 0 is not equal to \"one\", or\n"
            "element 1 is not equal to \"two\"", DescribeNegation(m));
}

TEST(ElementsAreTest, DoesNotExplainTrivialMatch) {
  Matcher<const list<int>& > m = ElementsAre(1, Ne(2));

  list<int> test_list;
  test_list.push_back(1);
  test_list.push_back(3);
  EXPECT_EQ("", Explain(m, test_list));  // No need to explain anything.
}

TEST(ElementsAreTest, ExplainsNonTrivialMatch) {
  Matcher<const vector<int>& > m =
      ElementsAre(GreaterThan(1), 0, GreaterThan(2));

  const int a[] = { 10, 0, 100 };
  vector<int> test_vector(a, a + GMOCK_ARRAY_SIZE_(a));
  EXPECT_EQ("element 0 is 9 more than 1,\n"
            "element 2 is 98 more than 2", Explain(m, test_vector));
}

TEST(ElementsAreTest, CanExplainMismatchWrongSize) {
  Matcher<const list<int>& > m = ElementsAre(1, 3);

  list<int> test_list;
  // No need to explain when the container is empty.
  EXPECT_EQ("", Explain(m, test_list));

  test_list.push_back(1);
  EXPECT_EQ("has 1 element", Explain(m, test_list));
}

TEST(ElementsAreTest, CanExplainMismatchRightSize) {
  Matcher<const vector<int>& > m = ElementsAre(1, GreaterThan(5));

  vector<int> v;
  v.push_back(2);
  v.push_back(1);
  EXPECT_EQ("element 0 doesn't match", Explain(m, v));

  v[0] = 1;
  EXPECT_EQ("element 1 doesn't match (is 4 less than 5)", Explain(m, v));
}

TEST(ElementsAreTest, MatchesOneElementVector) {
  vector<string> test_vector;
  test_vector.push_back("test string");

  EXPECT_THAT(test_vector, ElementsAre(StrEq("test string")));
}

TEST(ElementsAreTest, MatchesOneElementList) {
  list<string> test_list;
  test_list.push_back("test string");

  EXPECT_THAT(test_list, ElementsAre("test string"));
}

TEST(ElementsAreTest, MatchesThreeElementVector) {
  vector<string> test_vector;
  test_vector.push_back("one");
  test_vector.push_back("two");
  test_vector.push_back("three");

  EXPECT_THAT(test_vector, ElementsAre("one", StrEq("two"), _));
}

TEST(ElementsAreTest, MatchesOneElementEqMatcher) {
  vector<int> test_vector;
  test_vector.push_back(4);

  EXPECT_THAT(test_vector, ElementsAre(Eq(4)));
}

TEST(ElementsAreTest, MatchesOneElementAnyMatcher) {
  vector<int> test_vector;
  test_vector.push_back(4);

  EXPECT_THAT(test_vector, ElementsAre(_));
}

TEST(ElementsAreTest, MatchesOneElementValue) {
  vector<int> test_vector;
  test_vector.push_back(4);

  EXPECT_THAT(test_vector, ElementsAre(4));
}

TEST(ElementsAreTest, MatchesThreeElementsMixedMatchers) {
  vector<int> test_vector;
  test_vector.push_back(1);
  test_vector.push_back(2);
  test_vector.push_back(3);

  EXPECT_THAT(test_vector, ElementsAre(1, Eq(2), _));
}

TEST(ElementsAreTest, MatchesTenElementVector) {
  const int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  vector<int> test_vector(a, a + GMOCK_ARRAY_SIZE_(a));

  EXPECT_THAT(test_vector,
              // The element list can contain values and/or matchers
              // of different types.
              ElementsAre(0, Ge(0), _, 3, 4, Ne(2), Eq(6), 7, 8, _));
}

TEST(ElementsAreTest, DoesNotMatchWrongSize) {
  vector<string> test_vector;
  test_vector.push_back("test string");
  test_vector.push_back("test string");

  Matcher<vector<string> > m = ElementsAre(StrEq("test string"));
  EXPECT_FALSE(m.Matches(test_vector));
}

TEST(ElementsAreTest, DoesNotMatchWrongValue) {
  vector<string> test_vector;
  test_vector.push_back("other string");

  Matcher<vector<string> > m = ElementsAre(StrEq("test string"));
  EXPECT_FALSE(m.Matches(test_vector));
}

TEST(ElementsAreTest, DoesNotMatchWrongOrder) {
  vector<string> test_vector;
  test_vector.push_back("one");
  test_vector.push_back("three");
  test_vector.push_back("two");

  Matcher<vector<string> > m = ElementsAre(
    StrEq("one"), StrEq("two"), StrEq("three"));
  EXPECT_FALSE(m.Matches(test_vector));
}

TEST(ElementsAreTest, WorksForNestedContainer) {
  const char* strings[] = {
    "Hi",
    "world"
  };

  vector<list<char> > nested;
  for (int i = 0; i < GMOCK_ARRAY_SIZE_(strings); i++) {
    nested.push_back(list<char>(strings[i], strings[i] + strlen(strings[i])));
  }

  EXPECT_THAT(nested, ElementsAre(ElementsAre('H', Ne('e')),
                                  ElementsAre('w', 'o', _, _, 'd')));
  EXPECT_THAT(nested, Not(ElementsAre(ElementsAre('H', 'e'),
                                      ElementsAre('w', 'o', _, _, 'd'))));
}

TEST(ElementsAreTest, WorksWithByRefElementMatchers) {
  int a[] = { 0, 1, 2 };
  vector<int> v(a, a + GMOCK_ARRAY_SIZE_(a));

  EXPECT_THAT(v, ElementsAre(Ref(v[0]), Ref(v[1]), Ref(v[2])));
  EXPECT_THAT(v, Not(ElementsAre(Ref(v[0]), Ref(v[1]), Ref(a[2]))));
}

TEST(ElementsAreTest, WorksWithContainerPointerUsingPointee) {
  int a[] = { 0, 1, 2 };
  vector<int> v(a, a + GMOCK_ARRAY_SIZE_(a));

  EXPECT_THAT(&v, Pointee(ElementsAre(0, 1, _)));
  EXPECT_THAT(&v, Not(Pointee(ElementsAre(0, _, 3))));
}

// Tests for ElementsAreArray().  Since ElementsAreArray() shares most
// of the implementation with ElementsAre(), we don't test it as
// thoroughly here.

TEST(ElementsAreArrayTest, CanBeCreatedWithValueArray) {
  const int a[] = { 1, 2, 3 };

  vector<int> test_vector(a, a + GMOCK_ARRAY_SIZE_(a));
  EXPECT_THAT(test_vector, ElementsAreArray(a));

  test_vector[2] = 0;
  EXPECT_THAT(test_vector, Not(ElementsAreArray(a)));
}

TEST(ElementsAreArrayTest, CanBeCreatedWithArraySize) {
  const char* a[] = { "one", "two", "three" };

  vector<string> test_vector(a, a + GMOCK_ARRAY_SIZE_(a));
  EXPECT_THAT(test_vector, ElementsAreArray(a, GMOCK_ARRAY_SIZE_(a)));

  const char** p = a;
  test_vector[0] = "1";
  EXPECT_THAT(test_vector, Not(ElementsAreArray(p, GMOCK_ARRAY_SIZE_(a))));
}

TEST(ElementsAreArrayTest, CanBeCreatedWithoutArraySize) {
  const char* a[] = { "one", "two", "three" };

  vector<string> test_vector(a, a + GMOCK_ARRAY_SIZE_(a));
  EXPECT_THAT(test_vector, ElementsAreArray(a));

  test_vector[0] = "1";
  EXPECT_THAT(test_vector, Not(ElementsAreArray(a)));
}

TEST(ElementsAreArrayTest, CanBeCreatedWithMatcherArray) {
  const Matcher<string> kMatcherArray[] =
    { StrEq("one"), StrEq("two"), StrEq("three") };

  vector<string> test_vector;
  test_vector.push_back("one");
  test_vector.push_back("two");
  test_vector.push_back("three");
  EXPECT_THAT(test_vector, ElementsAreArray(kMatcherArray));

  test_vector.push_back("three");
  EXPECT_THAT(test_vector, Not(ElementsAreArray(kMatcherArray)));
}

}  // namespace