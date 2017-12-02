// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Björn Buchhold <buchhold@cs.uni-freiburg.de>,
// Patrick Brosi <brosi@cs.uni-freiburg.de>.

#ifndef INVERTEDINDEX_H_
#define INVERTEDINDEX_H_

#include <unordered_map>
#include <string>
#include <vector>

using std::string;
using std::vector;

typedef std::unordered_map<string, vector<uint64_t>> InvLists;

// A simple inverted index as explained in lecture 1.
class InvertedIndex {
 public:
  // Constructs the inverted index from given file (one record per line).
  void readFromFile(const string& fileName);

  // Returns the inverted lists.
  const InvLists& getInvertedLists() const;

  // Returns intersection of two given sorted inverted lists
  std::vector<uint64_t> intersect(std::vector<uint64_t> v1,
                                  std::vector<uint64_t> v2);

  // Fetches intersection of inverted lists of given keywords
  std::vector<uint64_t> process_querry(string querry);

 private:
  // In `InvertedIndex::readFromFile` value of `recordId`,
  // which type is uint64_t, is pushed to vector.
  // Thus, I decided to unify those types (int --> uint64_t)
  std::unordered_map<string, vector<uint64_t>> _invertedLists;

  // Returns the lowercase version of a string.
  inline static string getLowercase(string orig);
};

// Print inverted list in human readable format
std::ostream& operator<<(std::ostream& os, const std::vector<uint64_t> v);
std::ostream& operator<<(std::ostream&, const InvertedIndex &ii);


#endif  // INVERTEDINDEX_H_
