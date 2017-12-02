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
using std::pair;

typedef std::unordered_map<string, vector<pair<uint64_t, double>>> InvLists;
typedef std::pair<uint64_t, double> Posting;

// A simple inverted index as explained in lecture 1.
class InvertedIndex {
 public:
  // Constructs the inverted index from given file (one record per line).
  void readFromFile(const string& fileName, double b, double k);

  // Returns the inverted lists.
  const InvLists& getInvertedLists() const;

  // Returns intersection of two given sorted inverted lists
  std::vector<Posting> merge(std::vector<Posting> v1,
                             std::vector<Posting> v2);

  // Fetches intersection of inverted lists of given keywords
  std::vector<Posting> process_querry(string querry, bool use_refinements);

  // Sets inverted lists (for ex 1.3; shorter than writing constructs)
  void setInvertedLists(InvLists il) {
    _invertedLists = il;
  }

 private:
  // In `InvertedIndex::readFromFile` value of `recordId`,
  // which type is uint64_t, is pushed to vector.
  // Thus, I decided to unify those types (int --> uint64_t)
  std::unordered_map<string, vector<pair<uint64_t, double>>> _invertedLists;

  // Vector of documents length (in words). Indexed from 1!
  std::vector<uint64_t> _docLen;

  // Average document length
  double _avgDocLen;

  // Returns the lowercase version of a string.
  inline static string getLowercase(string orig);
};

// Print inverted list in human readable format
std::ostream& operator<<(std::ostream& os, const std::vector<Posting> v);
std::ostream& operator<<(std::ostream&, const InvertedIndex &ii);


#endif  // INVERTEDINDEX_H_
