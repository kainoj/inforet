// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Björn Buchhold <buchholb@cs.uni-freiburg.de>,
//          Patrick Brosi <brosi@cs.uni-freiburg.de>,
//          Claudius Korzen <korzen@cs.uni-freiburg.de>.

#include <gtest/gtest.h>
#include "./QGramIndex.h"


// _____________________________________________________________________________
TEST(QGramIndexTest, testBuildFromFile) {
  QGramIndex index(3);
  index.buildFromFile("example.tsv");

  ASSERT_EQ(7, index._invertedLists.size());

//   "$$b": [2],
   //     "$$f": [1],
   //     "$br": [2],
   //     "$fr": [1],
   //     "bre": [2],
   //     "fre": [1],
   //     "rei": [1, 2]

  ASSERT_EQ(1, index._invertedLists["$$b"].size());
  ASSERT_EQ(2, index._invertedLists["$$b"][0]);
  ASSERT_EQ(1, index._invertedLists["$$f"].size());
  ASSERT_EQ(1, index._invertedLists["$$f"][0]);
  ASSERT_EQ(1, index._invertedLists["$br"].size());
  ASSERT_EQ(2, index._invertedLists["$br"][0]);
  ASSERT_EQ(1, index._invertedLists["$fr"].size());
  ASSERT_EQ(1, index._invertedLists["$fr"][0]);
  ASSERT_EQ(1, index._invertedLists["bre"].size());
  ASSERT_EQ(2, index._invertedLists["bre"][0]);
  ASSERT_EQ(1, index._invertedLists["fre"].size());
  ASSERT_EQ(1, index._invertedLists["fre"][0]);
  // ASSERT_EQ(2, index._invertedLists["ei$"].size());
  // ASSERT_EQ(1, index._invertedLists["ei$"][0]);
  // ASSERT_EQ(2, index._invertedLists["ei$"][1]);
  // ASSERT_EQ(2, index._invertedLists["i$$"].size());
  // ASSERT_EQ(1, index._invertedLists["i$$"][0]);
  // ASSERT_EQ(2, index._invertedLists["i$$"][1]);
  ASSERT_EQ(2, index._invertedLists["rei"].size());
  ASSERT_EQ(1, index._invertedLists["rei"][0]);
  ASSERT_EQ(2, index._invertedLists["rei"][1]);

  // Entities test
  ASSERT_EQ(2, index.entities.size());

  ASSERT_EQ("frei",   index.entities[0].getName());
  ASSERT_EQ(3,        index.entities[0].getScore());
  ASSERT_EQ("a word", index.entities[0].getDesc());

  ASSERT_EQ("brei",   index.entities[1].getName());
  ASSERT_EQ(2,        index.entities[1].getScore());
  ASSERT_EQ("another word", index.entities[1].getDesc());
}

// _____________________________________________________________________________
TEST(QGramIndexTest, testComputeQGrams) {
  QGramIndex index(3);
  auto qgrams = index.computeQGrams("freiburg");
  ASSERT_EQ(qgrams.size(), 8);
  ASSERT_EQ(qgrams[0], "$$f");
  ASSERT_EQ(qgrams[1], "$fr");
  ASSERT_EQ(qgrams[2], "fre");
  ASSERT_EQ(qgrams[3], "rei");
  ASSERT_EQ(qgrams[4], "eib");
  ASSERT_EQ(qgrams[5], "ibu");
  ASSERT_EQ(qgrams[6], "bur");
  ASSERT_EQ(qgrams[7], "urg");
  // ASSERT_EQ(qgrams[8], "rg$");
  // ASSERT_EQ(qgrams[9], "g$$");
}

// _____________________________________________________________________________
TEST(QGramIndexTest, testNormalizeString) {
  ASSERT_EQ("freiburg", QGramIndex::normalize("Frei, burg !!"));
  ASSERT_EQ("freiburg", QGramIndex::normalize("freiburg"));
}


// _____________________________________________________________________________
TEST(QGramIndexTest, testMergeLists) {
  QGramIndex index(3);
  auto res = index.mergeLists({1, 1, 3, 5}, {2, 3, 3, 9, 9});
  std::vector<size_t> expected_res = {1, 1, 2, 3, 3, 3, 5, 9, 9};
  ASSERT_EQ(res.size(), expected_res.size());
  for (size_t i=0; i < res.size(); i++) {
    ASSERT_EQ(res[i], expected_res[i]);
  }
}

// _____________________________________________________________________________
TEST(QGramIndexTest, testPrefixEditDistance) {
  QGramIndex index(3);
  ASSERT_EQ(0, index.prefixEditDistance("frei", "frei", 0));
  ASSERT_EQ(0, index.prefixEditDistance("frei", "freiburg", 0));
  ASSERT_EQ(1, index.prefixEditDistance("frei", "breifurg", 1));
  ASSERT_EQ(3, index.prefixEditDistance("freiburg", "stuttgart", 2));
}

// _____________________________________________________________________________
TEST(QGramIndexTest, testFindMatches) {
  QGramIndex index(3);
  index.buildFromFile("example.tsv");

  auto res = index.findMatches("frei", 0);
  ASSERT_EQ(1, res.second);
  ASSERT_EQ(1, res.first.size());
  ASSERT_EQ(0, res.first[0].second);
  ASSERT_EQ("frei",   res.first[0].first.getName());
  ASSERT_EQ(3,        res.first[0].first.getScore());
  ASSERT_EQ("a word", res.first[0].first.getDesc());

  res = index.findMatches("frei", 2);
  ASSERT_EQ(2, res.second);
  ASSERT_EQ(2, res.first.size());
  ASSERT_EQ(0, res.first[0].second);  // PED
  ASSERT_EQ("frei",   res.first[0].first.getName());
  ASSERT_EQ(3,        res.first[0].first.getScore());
  ASSERT_EQ("a word", res.first[0].first.getDesc());

  ASSERT_EQ(1,        res.first[1].second);
  ASSERT_EQ("brei",   res.first[1].first.getName());
  ASSERT_EQ(2,        res.first[1].first.getScore());
  ASSERT_EQ("another word", res.first[1].first.getDesc());

  res =  index.findMatches("freibu", 2);
  ASSERT_EQ(2, res.second);
  ASSERT_EQ(1, res.first.size());
  ASSERT_EQ(2, res.first[0].second);
  ASSERT_EQ("frei",   res.first[0].first.getName());
  ASSERT_EQ(3,        res.first[0].first.getScore());
  ASSERT_EQ("a word", res.first[0].first.getDesc());
}


// _____________________________________________________________________________
TEST(QGramIndex, testRankMatches) {
  QGramIndex index(3);
  std::vector<std::pair<Entity, size_t>> e =
          {{Entity{"foo", 3, "word 0"}, 2},
           {Entity{"bar", 7, "word 1"}, 0},
           {Entity{"baz", 2, "word 2"}, 1},
           {Entity{"boo", 5, "word 3"}, 1},
        };
  std::vector<std::pair<Entity, size_t>> expected =
          {{Entity{"bar", 7, "word 1"}, 0},
           {Entity{"boo", 5, "word 3"}, 1},
           {Entity{"baz", 2, "word 2"}, 1},
           {Entity{"foo", 3, "word 0"}, 2},
        };
  e = index.rankMatches(e);
  for (size_t i = 0; i < e.size(); i++) {
    ASSERT_EQ(e[i].first, expected[i].first);
    ASSERT_EQ(e[i].second, expected[i].second);
  }
}
