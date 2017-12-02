// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@cs.uni-freiburg.de>,
//          Claudius Korzen <korzen@cs.uni-freiburg.de>.

#include "./PostingList.h"
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <limits>

// _____________________________________________________________________________
void PostingList::readFromFile(const std::string& fileName) {
  std::ifstream fstream(fileName.c_str());
  std::string curLine;

  // Read the number of postings in the file from the first line.
  std::getline(fstream, curLine);
  size_t numPosts;
  std::stringstream s(curLine);
  s >> numPosts;

  // Reserve enough space for the postings.
  reserve(numPosts);

  // Read the file line by line, each in the format <id>WHITESPACE<score>.
  size_t i = 0;
  while (std::getline(fstream, curLine) && i < numPosts) {
    i++;
    std::stringstream s(curLine);
    size_t docId;
    size_t score;

    // Split the line into <id> and <score>.
    s >> docId;
    s >> score;

    // Add the posting (id, score) to this list.
    addPosting(docId, score);
  }
}

// _____________________________________________________________________________
PostingList PostingList::intersectBaseline(const PostingList& l1,
    const PostingList& l2) {
  PostingList result;
  result.reserve(std::min(l1.size(), l2.size()));

  size_t i1 = 0;
  size_t i2 = 0;

  while (i1 < l1.size() && i2 < l2.size()) {
    while (i1 < l1.size() && l1.getId(i1) < l2.getId(i2)) {
      i1++;
    }

    if (i1 == l1.size()) {
      break;
    }

    while (i2 < l2.size() && l2.getId(i2) < l1.getId(i1)) {
      i2++;
    }

    if (i2 == l2.size()) {
      break;
    }

    if (l1.getId(i1) == l2.getId(i2)) {
      result.addPosting(l1.getId(i1), l1.getScore(i1) + l2.getScore(i2));
      i1++;
      i2++;
    }
  }
  return result;
}


// _____________________________________________________________________________
PostingList PostingList::intersect2(const PostingList& A,
    const PostingList& B) {
  PostingList result;
  result.reserve(std::min(A.size(), B.size()));

  size_t i = 0;
  size_t j = 0;

  while (i < A.size() && j < B.size()) {
    if (A.getId(i) == B.getId(j)) {
      result.addPosting(A.getId(i), A.getScore(i)+B.getScore(j));
      i++;
      j++;
    } else {
      if (A.getId(i) < B.getId(j))
        i++;
      else
        j++;
    }
  }
  return result;
}

// _____________________________________________________________________________
PostingList PostingList::intersect(const PostingList& A,
    const PostingList& B) {
  // Implement a new method for intersecting two posting lists that
  // uses at least three non-trivial ideas presented in the lecture. The goal
  // is to beat the baseline implementation for all scenarios of the exercise
  // sheet. Note that you can also implement several algorithms and switch
  // between them depending on the sizes of the input lists (or depending on
  // any information that you find to be useful). Note: Each implemented
  // method must pass the test case provided for the baseline implementation.

  // I changed variables names to A,B for better readability.

  // Make A a shorter vector
  if (A.size() > B.size()) {
    return PostingList::intersect(B, A);
  }
  PostingList res;
  res.reserve(A.size());


  size_t a = 0;  // Binary search will be in
  size_t b = 1;  // range [a,b] of B;
  size_t s = 0;  // s ← (a+b)/2
  size_t i = 0;  // index of current element in A
  while (true) {
    if ( A.getId(i) == INF )
      return res;
    b = 1;
    // Gallop - find upper bounds
    while ((a+b) <= B.size() && B.getId(a+b) < A.getId(i)) b*=2;
    b = std::min(a+b, B.size());
    while ( a < b ) {
      s = (a+b)/2;
      if (B.getId(s) < A.getId(i))
        a = s+1;
      else
        b = s;
    }
    if ( B.getId(a) == A.getId(i) ) {
      res.addPosting(A.getId(i), A.getScore(i) + B.getScore(a));
    }
    i++;
  }
  return res;
}

// _____________________________________________________________________________
size_t PostingList::bin_search(size_t a, size_t b, size_t id) const {
  size_t s;
  while ( a < b ) {
    s = (a+b)/2;
    if (ids[s] < id)
      a = s+1;
    else
      b = s;
  }
  if ( ids[a] == id ) return a;
  return INF;
}

// _____________________________________________________________________________
void PostingList::reserve(size_t n) {
  ids.reserve(n);
  scores.reserve(n);
  capacity = n;
  numPostings = 0;
}

// _____________________________________________________________________________
void PostingList::addPosting(size_t id, size_t score) {
  ids.push_back(id);
  scores.push_back(score);
  numPostings++;
}

// _____________________________________________________________________________
size_t PostingList::getId(size_t i) const {
  return ids[i];
}

// _____________________________________________________________________________
size_t PostingList::getScore(size_t i) const {
  return scores[i];
}

// _____________________________________________________________________________
size_t PostingList::size() const {
  return numPostings;
}
