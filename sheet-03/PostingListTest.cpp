// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@cs.uni-freiburg.de>,
//          Claudius Korzen <korzen@cs.uni-freiburg.de>.

#include <gtest/gtest.h>
#include "./PostingList.h"

// _____________________________________________________________________________
TEST(PostingList, readFromFile) {
  PostingList list1;
  list1.readFromFile("example1.txt");
  ASSERT_EQ(3, list1.size());
  ASSERT_EQ(2, list1.getId(0));
  ASSERT_EQ(3, list1.getId(1));
  ASSERT_EQ(6, list1.getId(2));
  ASSERT_EQ(5, list1.getScore(0));
  ASSERT_EQ(1, list1.getScore(1));
  ASSERT_EQ(2, list1.getScore(2));

  PostingList list2;
  list2.readFromFile("example2.txt");
  ASSERT_EQ(4, list2.size());
  ASSERT_EQ(1, list2.getId(0));
  ASSERT_EQ(2, list2.getId(1));
  ASSERT_EQ(4, list2.getId(2));
  ASSERT_EQ(6, list2.getId(3));
  ASSERT_EQ(1, list2.getScore(0));
  ASSERT_EQ(4, list2.getScore(1));
  ASSERT_EQ(3, list2.getScore(2));
  ASSERT_EQ(3, list2.getScore(3));

  PostingList list3;
  list3.readFromFile("example3.txt");
  ASSERT_EQ(2, list3.size());
  ASSERT_EQ(5, list3.getId(0));
  ASSERT_EQ(7, list3.getId(1));
  ASSERT_EQ(1, list3.getScore(0));
  ASSERT_EQ(2, list3.getScore(1));
}

// _____________________________________________________________________________
TEST(PostingList, intersectBaseline) {
  PostingList list1;
  list1.readFromFile("example1.txt");
  PostingList list2;
  list2.readFromFile("example2.txt");
  PostingList list3;
  list3.readFromFile("example3.txt");

  PostingList result1 = PostingList::intersectBaseline(list1, list2);
  ASSERT_EQ(2, result1.size());
  ASSERT_EQ(2, result1.getId(0));
  ASSERT_EQ(6, result1.getId(1));
  ASSERT_EQ(9, result1.getScore(0));
  ASSERT_EQ(5, result1.getScore(1));

  PostingList result2 = PostingList::intersectBaseline(list1, list3);
  ASSERT_EQ(0, result2.size());
}

/**
 * +++ IMPORTANT +++
 *
 * You have to implement tests for each new method you add to the PostingList
 * class.
 *
 * In particular, your "intersect" method should run the test for the
 * intersectBaseline() method above successfully.
 *
 * Please note: If you add several intersection methods with different
 * strategies, EACH of them must must pass the test case provided for the
 * baseline implementation.
 **/

// _____________________________________________________________________________
TEST(PostingList, bin_search) {
  PostingList p;
  int arr[] = {1, 3, 5, 7, 9};
  for (int i = 0; i < 5; i++)
    p.addPosting(arr[i], 42);

  for (size_t k = 0; k < 5; k++)
    ASSERT_EQ(arr[k]/2, p.bin_search(0, 4, arr[k]));

  for (size_t k = 0; k < 5; k++)
    ASSERT_EQ(INF, p.bin_search(0, 4, arr[k]+1));

  for (size_t k = 0; k <= 3; k++) {
    ASSERT_EQ(3, p.bin_search(k, 3, 7));
    ASSERT_EQ(3, p.bin_search(k, 4, 7));
    ASSERT_EQ(3, p.bin_search(k, 4, 7));
    ASSERT_EQ(INF, p.bin_search(4, k, 7));
  }
}


// _____________________________________________________________________________
TEST(PostingList, intersect) {
  PostingList list1;
  list1.readFromFile("example1.txt");
  PostingList list2;
  list2.readFromFile("example2.txt");
  PostingList list3;
  list3.readFromFile("example3.txt");

  PostingList result1 = PostingList::intersect(list1, list2);
  ASSERT_EQ(2, result1.size());
  ASSERT_EQ(2, result1.getId(0));
  ASSERT_EQ(6, result1.getId(1));
  ASSERT_EQ(9, result1.getScore(0));
  ASSERT_EQ(5, result1.getScore(1));

  // PostingList result2 = PostingList::intersect(list1, list3);
  // ASSERT_EQ(0, result2.size());
}
