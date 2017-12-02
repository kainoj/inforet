// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Björn Buchhold <buchhold@cs.uni-freiburg.de>,
// Patrick Brosi <brosi@cs.uni-freiburg.de>.

#include <gtest/gtest.h>
#include "./InvertedIndex.h"

/*
* In all cases bellow, contenet of
* inverted lists is indexed from 1
*/

// _____________________________________________________________________________
TEST(InvertedIndexTest, readFromFile) {
  InvertedIndex ii;
  ii.readFromFile("example.txt");
  ASSERT_EQ(7, ii.getInvertedLists().size());
  ASSERT_EQ(1, ii.getInvertedLists().find("doc")->second[0]);
  ASSERT_EQ(2, ii.getInvertedLists().find("doc")->second[1]);
  ASSERT_EQ(3, ii.getInvertedLists().find("doc")->second[2]);
  ASSERT_EQ(1, ii.getInvertedLists().find("0")->second[0]);
  ASSERT_EQ(2, ii.getInvertedLists().find("1")->second[0]);
  ASSERT_EQ(3, ii.getInvertedLists().find("2")->second[0]);
  ASSERT_EQ(1, ii.getInvertedLists().find("movie")->second[0]);
  ASSERT_EQ(3, ii.getInvertedLists().find("movie")->second[1]);
  ASSERT_EQ(2, ii.getInvertedLists().find("film")->second[0]);
}

// _____________________________________________________________________________
TEST(InvertedIndexTest, intersect) {
  InvertedIndex ii;
  ASSERT_EQ(0, ii.intersect({1, 5, 7}, {2, 4}).size());
  ASSERT_EQ(3, ii.intersect({1, 2, 5, 7}, {1, 3, 5, 6, 7, 9}).size());
  ASSERT_EQ(1, ii.intersect({1, 2, 5, 7}, {1, 3, 5, 6, 7, 9}).at(0));
  ASSERT_EQ(5, ii.intersect({1, 2, 5, 7}, {1, 3, 5, 6, 7, 9}).at(1));
  ASSERT_EQ(7, ii.intersect({1, 2, 5, 7}, {1, 3, 5, 6, 7, 9}).at(2));
}

// _____________________________________________________________________________
TEST(InvertedIndexTest, process_querry) {
  InvertedIndex ii;
  ii.readFromFile("example.txt");
  ASSERT_EQ(0, ii.process_querry("").size());
  ASSERT_EQ(2, ii.process_querry("doc movie").size());
  ASSERT_EQ(1, ii.process_querry("doc movie")[0]);
  ASSERT_EQ(3, ii.process_querry("doc movie")[1]);
  ASSERT_EQ(0, ii.process_querry("doc movie comedy").size());
}
