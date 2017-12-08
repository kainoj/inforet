
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

// An entity in the q-gram index.
struct Entity {
  // The name of this entity.
  std::string name;
  // The score of this entity.
  size_t score;
  // The description of this entity.
  std::string description;
  // The Wikipedia url of this entity.
  std::string wikipediaUrl;
  // The Wikidata id of this entity.
  std::string wikidataId;
  // The synonyms of this entity.
  std::vector<std::string> synonyms;
  // The image url of this entity.
  std::string imageUrl;
  // The prefix edit distance when this entity is part of a query result.
  size_t ped;
  // The matched synonym when this entity is part of a query result due to a
  // matching synonym.
  std::string matchedSynonym;

  Entity();

  // Creates a new entity.
  Entity(const std::string& name, size_t score) : name(name), score(score) {}

  // Creates a new entity.
  Entity(const std::string& name, size_t score, const std::string& description,
      const std::string& wikipediaUrl, const std::string& wikidataId,
      const std::vector<std::string>& synonyms, const std::string& imageUrl) :
      name(name), score(score), description(description),
      wikipediaUrl(wikipediaUrl), wikidataId(wikidataId), synonyms(synonyms),
      imageUrl(imageUrl) {}
};

inline bool _entityComparator(const Entity& first, const Entity& second) {
  if (first.ped < second.ped) return true;
  if (first.ped == second.ped) return first.score > second.score;
  return false;
}

// A simple q-gram index as explained in lecture 5.
class QGramIndex {
 public:
  // Creates an empty q-gram index.
  explicit QGramIndex(size_t q, bool withSynonyms) : _q(q),
      _withSynonyms(withSynonyms) {
    for (size_t i = 0; i < q - 1; ++i) { _padding += '$'; }
  }

  // Builds the index from the given file (one line per entity, see ES5).
  void buildFromFile(const std::string& fileName);

  // Merges the given inverted lists.
  static std::vector<std::pair<size_t, size_t> > mergeLists(
      const std::vector<const std::vector<size_t>* >& lists);

  // Computes the prefix edit distance PED(x,y) for the two given strings x and
  // y. Returns PED(x,y) if it is smaller or equal to the given delta; delta + 1
  // otherwise.
  static size_t prefixEditDistance(const std::string& x, const std::string& y,
      size_t delta);

  // Finds all entities y with PED(x, y) <= delta for a given integer delta and
  // a given (normalized) prefix x.
  std::pair<std::vector<Entity>, size_t> findMatches(const std::string& prefix)
      const;

  // Ranks the given list of entities (PED, s), where PED is the PED value and s
  // is the popularity score of an entity.
  static std::vector<Entity> rankMatches(const std::vector<Entity>& matches);

  // Compute q-grams for padded, normalized version of given string.
  std::vector<std::string> computeQGrams(const std::string& word) const;

  // Normalize the given string (remove non-word characters and lower case).
  static std::string normalize(const std::string& str);

  // Splits the given string on the given delimiter.
  static std::vector<std::string> split(const std::string& text, char sep);

  // The value of q.
  size_t _q;

  // The padding (q-1 times $).
  std::string _padding;

  // The inverted lists.
  std::unordered_map<std::string, std::vector<size_t>> _invertedLists;

  // The list of entities.
  std::vector<Entity> _entities;

  // The boolean flag that indicates whether to use synonyms or not.
  bool _withSynonyms;
};

#endif  // QGRAMINDEX_H_
