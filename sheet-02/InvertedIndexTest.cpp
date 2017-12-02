// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Björn Buchhold <buchhold@cs.uni-freiburg.de>,
// Patrick Brosi <brosi@cs.uni-freiburg.de>.

#include <gtest/gtest.h>
#include "./InvertedIndex.h"
#include "./EvaluateInvertedIndex.h"

/*
 * In all cases bellow, contenet of
 * inverted lists is indexed from 1
 */

const double EPS = 0.001;  // Desired precision in calculations

/*
 * Since in both 'readFromFile' test cases 1st values in pairs (doc number)
 * are the same, I've decided to split those two cases into three:
 *   - readFormFile  - tests doc number only
 *   - readFromFile1 - tests BM25 for b = 0, k = inf
 *   - readFromFile2 - tests BM25 for b = 0.75, k = 1.75
 */

// _____________________________________________________________________________
// Test case: docs numbers _____________________________________________________
TEST(InvertedIndexTest, readFromFile) {
  InvertedIndex ii;
  ii.readFromFile("example.txt", 0.75 , 1.75);
  ASSERT_EQ(6, ii.getInvertedLists().size());

  ASSERT_EQ(1, ii.getInvertedLists().find("animated")->second[0].first);
  ASSERT_EQ(2, ii.getInvertedLists().find("animated")->second[1].first);
  ASSERT_EQ(4, ii.getInvertedLists().find("animated")->second[2].first);

  ASSERT_EQ(3, ii.getInvertedLists().find("animation")->second[0].first);

  ASSERT_EQ(2, ii.getInvertedLists().find("film")->second[0].first);
  ASSERT_EQ(4, ii.getInvertedLists().find("film")->second[1].first);

  ASSERT_EQ(1, ii.getInvertedLists().find("movie")->second[0].first);
  ASSERT_EQ(2, ii.getInvertedLists().find("movie")->second[1].first);
  ASSERT_EQ(3, ii.getInvertedLists().find("movie")->second[2].first);
  ASSERT_EQ(4, ii.getInvertedLists().find("movie")->second[3].first);

  ASSERT_EQ(2, ii.getInvertedLists().find("non")->second[0].first);

  ASSERT_EQ(3, ii.getInvertedLists().find("short")->second[0].first);
  ASSERT_EQ(4, ii.getInvertedLists().find("short")->second[1].first);
}

// _____________________________________________________________________________
// Test case: BM25 with b = 0.75, k = 1.75 _____________________________________
TEST(InvertedIndexTest, readFromFile2) {
  InvertedIndex ii;
  ii.readFromFile("example.txt", 0.75 , 1.75);

  //  'animated':  [(1, 0.459), (2, 0.402),   (4, 0.358)]
  const double ans_animated[3] = {0.459, 0.402, 0.358};
  for (int i = 0; i < 3; i++) {
    ASSERT_NEAR(ans_animated[i],
                ii.getInvertedLists().find("animated")->second[i].second, EPS);
  }

  //  'animation': [(3, 2.211)]
  ASSERT_NEAR(2.211,
              ii.getInvertedLists().find("animation")->second[0].second, EPS);

  //  'film': [(2, 0.969), (4, 0.863)]
  ASSERT_NEAR(0.969, ii.getInvertedLists().find("film")->second[0].second, EPS);
  ASSERT_NEAR(0.863, ii.getInvertedLists().find("film")->second[1].second, EPS);

  // 'movie':   [(1, 0.0), (2, 0.0), (3, 0.0), (4, 0.0)],
  const double ans_movie[4] = {0.0, 0.0, 0.0, 0.0};
  for (int i = 0; i < 4; i++) {
    ASSERT_NEAR(ans_movie[i],
              ii.getInvertedLists().find("movie")->second[i].second,
              EPS);
  }

  // 'non':     [(2, 1.938)]
  ASSERT_NEAR(1.938, ii.getInvertedLists().find("non")->second[0].second, EPS);

  // 'short':   [(3, 1.106), (4, 1.313)]
  ASSERT_NEAR(1.106,
              ii.getInvertedLists().find("short")->second[0].second, EPS);
  ASSERT_NEAR(1.313,
              ii.getInvertedLists().find("short")->second[1].second, EPS);
}

// _____________________________________________________________________________
// Test case: BM25 with b=0, k=inf
TEST(InvertedIndexTest, readFromFile1) {
  InvertedIndex ii;
  ii.readFromFile("example.txt", 0 , std::numeric_limits<double>::max());

  //     'animated':  [(1, 0.415), (2, 0.415), (4, 0.415)],
  const double ans_animated[3] = {0.415, 0.415, 0.415};
  for (int i = 0; i < 3; i++) {
    ASSERT_NEAR(ans_animated[i],
                ii.getInvertedLists().find("animated")->second[i].second, EPS);
  }

  //     'animation': [(3, 2.0)]
  ASSERT_NEAR(2.0,
              ii.getInvertedLists().find("animation")->second[0].second, EPS);

  //     'film':      [(2, 1.0), (4, 1.0)]
  ASSERT_NEAR(1.0, ii.getInvertedLists().find("film")->second[0].second, EPS);
  ASSERT_NEAR(1.0, ii.getInvertedLists().find("film")->second[1].second, EPS);

  //     'movie':     [(1, 0.0), (2, 0.0), (3, 0.0), (4, 0.0)],
  ASSERT_NEAR(0.0, ii.getInvertedLists().find("movie")->second[0].second, EPS);
  ASSERT_NEAR(0.0, ii.getInvertedLists().find("movie")->second[1].second, EPS);
  ASSERT_NEAR(0.0, ii.getInvertedLists().find("movie")->second[2].second, EPS);
  ASSERT_NEAR(0.0, ii.getInvertedLists().find("movie")->second[3].second, EPS);

  //     'non':       [(2, 2.0)],
  ASSERT_NEAR(2.0, ii.getInvertedLists().find("non")->second[0].second, EPS);

  //     'short':     [(3, 1.0), (4, 2.0)]
  ASSERT_NEAR(1.0,
              ii.getInvertedLists().find("short")->second[0].second, EPS);
  ASSERT_NEAR(2.0,
              ii.getInvertedLists().find("short")->second[1].second, EPS);
}

// _____________________________________________________________________________
TEST(InvertedIndexTest, merge) {
  InvertedIndex ii;

  // TIP test
  auto m = ii.merge({{1, 2.1}, {5, 3.2}},
                    {{1, 1.7}, {2, 1.3}, {5, 3.3}});

  ASSERT_EQ(1, m[0].first);
  ASSERT_EQ(2, m[1].first);
  ASSERT_EQ(5, m[2].first);

  ASSERT_DOUBLE_EQ(3.8, m[0].second);
  ASSERT_DOUBLE_EQ(1.3, m[1].second);
  ASSERT_DOUBLE_EQ(6.5, m[2].second);

  // My very own test
  m = ii.merge({},
               {{1, 1.7}, {2, 1.3}, {5, 3.3}});

  ASSERT_EQ(1, m[0].first);
  ASSERT_EQ(2, m[1].first);
  ASSERT_EQ(5, m[2].first);

  ASSERT_DOUBLE_EQ(1.7, m[0].second);
  ASSERT_DOUBLE_EQ(1.3, m[1].second);
  ASSERT_DOUBLE_EQ(3.3, m[2].second);
}

// _____________________________________________________________________________
TEST(InvertedIndexTest, process_querry) {
  InvertedIndex ii;
  ii.setInvertedLists({
    {"foo", {{1, 0.2}, {3, 0.6} }},
    {"bar", {{1, 0.4}, {2, 0.7}, {3, 0.5} }},
    {"baz", {{2, 0.1}} }
  });
  auto res = ii.process_querry("foo bar", false);

  EXPECT_EQ(3, res.size());
  ASSERT_EQ(3, res[0].first);
  ASSERT_EQ(2, res[1].first);
  ASSERT_EQ(1, res[2].first);

  ASSERT_DOUBLE_EQ(1.1, res[0].second);
  ASSERT_DOUBLE_EQ(0.7, res[1].second);
  ASSERT_DOUBLE_EQ(0.6, res[2].second);
}

// _____________________________________________________________________________
TEST(EvaluateInvertedIndexTest, read_benchmark) {
  EvaluateInvertedIndex eii;
  unordered_map<string, set<int>> bench = {{"animated film", {1, 3, 4}},
                                              {"short film", {3, 4}}
                                              };
  ASSERT_EQ(bench, eii.read_benchmark("example-benchmark.txt"));
}

// _____________________________________________________________________________
TEST(EvaluateInvertedIndexTest, precision_at_k) {
  EvaluateInvertedIndex eii;
  ASSERT_DOUBLE_EQ(0.5,
                   eii.precision_at_k({5, 3, 6, 1, 2}, {1, 2, 5, 6, 7, 8}, 2));
  ASSERT_DOUBLE_EQ(0.75,
                   eii.precision_at_k({5, 3, 6, 1, 2}, {1, 2, 5, 6, 7, 8}, 4));
}

// _____________________________________________________________________________
TEST(EvaluateInvertedIndexTest, average_precision) {
  EvaluateInvertedIndex eii;
  ASSERT_DOUBLE_EQ(0.525,
                   eii.average_precision({7, 17, 9, 42, 5}, {5, 7, 12, 42}));
}

// _____________________________________________________________________________
TEST(EvaluateInvertedIndexTest, evaluate) {
  InvertedIndex ii;
  ii.readFromFile("example.txt", 0.75, 1.75);
  EvaluateInvertedIndex eii;
  auto benchmark = eii.read_benchmark("example-benchmark.txt");
  std::tuple<double, double, double> res = eii.evaluate(ii, benchmark, false);
  //  /   (0.667, 0.833, 0.694)
  ASSERT_NEAR(0.667, std::get<0>(res), EPS);
  ASSERT_NEAR(0.833, std::get<1>(res), EPS);
  ASSERT_NEAR(0.694, std::get<2>(res), EPS);
}
