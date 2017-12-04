
// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Björn Buchhold <buchholb@cs.uni-freiburg.de>,
//          Patrick Brosi <brosi@cs.uni-freiburg.de>,
//          Claudius Korzen <korzen@cs.uni-freiburg.de>.

#ifndef QGRAMINDEX_H_
#define QGRAMINDEX_H_

#include <unordered_map>
#include <string>
#include <vector>
#include "Entity.h"

using std::string;
using std::min;
using std::pair;
using std::vector;

// First steps towards a q-gram index, written during class.
class QGramIndex {
 public:
  explicit QGramIndex(size_t q) : _q(q) {
    for (size_t i = 0; i < q - 1; ++i) { _padding += '$'; }
  }

  // Build index from given file (one line per entity, see ES5).
  void buildFromFile(const std::string& fileName);

  // Compute q-grams for padded, normalized version of given string.
  std::vector<std::string> computeQGrams(const std::string& word) const;

  // Normalize the given string (remove non-word characters and lower case).
  static std::string normalize(const std::string& str);

  // Merge to lists with keeping duplicates
  std::vector<size_t> mergeLists(std::vector<size_t> a, std::vector<size_t> b);

  // Compute the prefix edit distance of the two given strings x and y and
  // return it if it is smaller or equal to the given δ. Otherwise return δ + 1
  size_t prefixEditDistance(std::string a, std::string b, size_t delta);

  vector<string> split(const std::string& text, char sep);

  // Find all entities y with PED(x, y) ≤ δ for the given
  // string x and a given integer δ.
  pair<vector<pair<Entity, size_t>>, size_t>
     findMatches(string x, size_t delta);

  // Rank the given set of pairs (entity, ped) by (ped, s)
  vector<pair<Entity, size_t>> rankMatches(vector<pair<Entity, size_t>> match);

  // Both find and rank matches wrapped in one function
  vector<pair<Entity, size_t>> findAndRankMatches(string x, size_t delta);

  // The q from the q-gram index.
  size_t _q;

  // The padding (q - 1 times $).
  std::string _padding;

  // The inverted lists (one per q-gram).
  std::unordered_map<std::string, std::vector<size_t>> _invertedLists;

  // Entities
  std::vector<Entity> entities;
};

#endif  // QGRAMINDEX_H_
