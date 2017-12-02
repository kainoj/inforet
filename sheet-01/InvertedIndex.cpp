// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Björn Buchhold <buchhold@cs.uni-freiburg.de>,
// Patrick Brosi <brosi@cs.uni-freiburg.de>.

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include "./InvertedIndex.h"

// _____________________________________________________________________________
void InvertedIndex::readFromFile(const string& fileName) {
  string line;
  std::ifstream in(fileName.c_str());
  uint64_t recordId = 0;

  while (std::getline(in, line)) {
    recordId++;

    size_t i = 0;
    size_t start = 0;
    while (i <= line.size()) {
      // The > 0 so that we don't split at non-ascii chars.
      // It is very important that one can search for "björn" :-).
      if (i == line.size() || (line[i] > 0 && !isalnum(line[i])))  {
        if (i > start) {
         string word = getLowercase(line.substr(start, i - start));
          // Automatically default constructs a list if the
          // word is seen for the first time.
          if (_invertedLists[word].empty()
            || _invertedLists[word].back() != recordId)
            _invertedLists[word].push_back(recordId);
        }
        start = i + 1;
      }
      ++i;
    }
  }
}

// ____________________________________________________________________________
inline string InvertedIndex::getLowercase(string orig) {
  std::transform(orig.begin(), orig.end(), orig.begin(), ::tolower);
  return orig;
}

// ____________________________________________________________________________
const InvLists& InvertedIndex::getInvertedLists() const {
  return _invertedLists;
}

// ____________________________________________________________________________
std::ostream& operator<<(std::ostream& os, const std::vector<uint64_t> v) {
  for (auto it = v.begin(); it != v.end(); ++it) {
    os << *it << " ";
  }
  return os;
}


// ____________________________________________________________________________
std::ostream& operator<<(std::ostream& os, const InvertedIndex &ii) {
  for (auto it = ii.getInvertedLists().begin();
      it != ii.getInvertedLists().end();
      ++it) {
    os << it->first << '\t';
    os << "[" << it->second << "]";
    os << std::endl;
  }
  return os;
}

// ____________________________________________________________________________
std::vector<uint64_t> InvertedIndex::intersect(std::vector<uint64_t> v1,
                                               std::vector<uint64_t> v2) {
  std::vector<uint64_t> res;
  size_t i = 0;  // Current index in v1
  size_t j = 0;  // Current index in v2
  while ( i < v1.size() && j < v2.size() ) {
    if (v1[i] == v2[j]) {
      res.push_back(v1[i]);
      i++;
      j++;
    } else {
      if (v1[i] < v2[j])
        i++;
      else
        j++;
    }
  }
  return res;
}

// ____________________________________________________________________________
std::vector<uint64_t> InvertedIndex::process_querry(string querry) {
  std::vector<uint64_t> querry_res;
  size_t i = 0;
  size_t start = 0;
  while (i <= querry.size()) {
    // As in readFromFile: The > 0 so that we don't split at non-ascii chars.
    if (i == querry.size() || (querry[i] > 0 && !isalnum(querry[i])))  {
      if (i > start) {
       string word = getLowercase(querry.substr(start, i - start));
       if (querry_res.empty()) {
         querry_res = _invertedLists[word];
       } else {
         querry_res = intersect(querry_res, _invertedLists[word]);
       }
      }
      start = i + 1;
    }
    ++i;
  }
  return querry_res;
}
