// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Björn Buchhold <buchholb@cs.uni-freiburg.de>,
//          Patrick Brosi <brosi@cs.uni-freiburg.de>,
//          Claudius Korzen <korzen@cs.uni-freiburg.de>.

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <queue>
#include <algorithm>
#include <limits>

#include "./QGramIndex.h"

// _____________________________________________________________________________
void QGramIndex::buildFromFile(const std::string& fileName) {
  std::ifstream in(fileName.c_str(), std::ios_base::in);

  // Ignore the first line (which includes the column headers).
  std::string line;
  std::getline(in, line);

  // Iterate through the remaining lines.
  while (std::getline(in, line)) {
    // Split the line and fetch the several fields.
    std::vector<std::string> parts = split(line, '\t');

    std::string name;
    if (parts.size() > 0) { name = parts[0]; }

    size_t score = 0;
    if (parts.size() > 1) { score = atoi(parts[1].c_str()); }

    std::string description;
    if (parts.size() > 2) { description = parts[2]; }

    std::string wikipediaUrl;
    if (parts.size() > 3) { wikipediaUrl = parts[3]; }

    std::string wikidataId;
    if (parts.size() > 4) { wikidataId = parts[4]; }

    std::vector<std::string> synonyms;
    if (parts.size() > 5) { synonyms = split(parts[5], ';'); }

    std::string imageUrl;
    if (parts.size() > 6) { imageUrl = parts[6]; }

    if (name.size() > 0) {
      // Compute the q-grams of the entity name and add them to the index.
      for (const std::string& qGram : computeQGrams(name)) {
        _invertedLists[qGram].push_back(_entities.size() + 1);
      }

      if (_withSynonyms) {
        // Compute the q-grams of the synonyms and add them to the index.
        for (const std::string& synonym : synonyms) {
          for (const std::string& qGram : computeQGrams(synonym)) {
            _invertedLists[qGram].push_back(_entities.size() + 1);
          }
        }
      }

      // Cache the entity.
      _entities.push_back(Entity(name, score, description, wikipediaUrl,
          wikidataId, synonyms, imageUrl));
    }
  }
}

// _____________________________________________________________________________
std::vector<std::pair<size_t, size_t> > QGramIndex::mergeLists(
      const std::vector<const std::vector<size_t>* >& lists) {
  std::vector<std::pair<size_t, size_t>> res;

  // The current positions in each list while merging the lists.
  // The element currentPositions[i] denotes the current position in list i.
  std::vector<size_t> currentPositions;
  currentPositions.resize(lists.size(), 0);

  // Introduce a priority queue with elements (element, listId), where
  // 'element' is an element in one of the lists and 'listId' is the index of
  // the corresponding list in 'lists'.
  std::priority_queue<std::pair<size_t, size_t>,
      std::vector<std::pair<size_t, size_t> >,
      std::greater<std::pair<size_t, size_t> > > pq;

  // Initially, put all first elements of each list into the queue.
  for (size_t i = 0; i < lists.size(); ++i) {
    if (lists[i]->size() > 0) {
      pq.push(std::pair<size_t, size_t>((*lists[i])[0], i));
      currentPositions[i]++;
    }
  }

  size_t curWordId = 0;
  size_t matchesForWordId = 0;

  // Process the priority queue element-wise.
  while (!pq.empty()) {
    // Check if the id of the top element was already seen.
    if (pq.top().first != curWordId) {
      // The id wasn't seen already, add the pending pair to the result.
      if (matchesForWordId > 0) {
        res.push_back(std::pair<size_t, size_t>(curWordId, matchesForWordId));
      }
      curWordId = pq.top().first;
      matchesForWordId = 0;
    }

    matchesForWordId++;
    size_t listId = pq.top().second;
    pq.pop();

    // Add the next element of the corresponding list to the queue.
    if (currentPositions[listId] < (*lists[listId]).size()) {
      pq.push(std::pair<size_t, size_t>(
          (*lists[listId])[currentPositions[listId]], listId));
      currentPositions[listId]++;
    }
  }

  // Don't forget the last wordId:
  if (matchesForWordId > 0) {
    res.push_back(std::pair<size_t, size_t>(curWordId, matchesForWordId));
  }

  return res;
}

// _____________________________________________________________________________
size_t QGramIndex::prefixEditDistance(const std::string& x,
    const std::string& y, size_t delta) {
  // Compute the dimensions of the matrix.
  size_t n = x.size() + 1;
  // Note that it is enough to compute the first |x| + δ + 1 columns.
  size_t m = std::min(x.size() + delta + 1, y.size() + 1);

  size_t matrix[n][m];

  // Initialize the first column.
  for (size_t i = 0; i < n; ++i) { matrix[i][0] = i; }
  // Initialize the first row.
  for (size_t i = 0; i < m; ++i) { matrix[0][i] = i; }

  // Compute the rest of the matrix.
  for (size_t i = 1; i < n; ++i) {
    for (size_t j = 1; j < m; ++j) {
      size_t s = (x[i - 1] == y[j - 1]) ? 0 : 1;
      size_t replaceCosts = matrix[i - 1][j - 1] + s;
      size_t addCosts = matrix[i][j - 1] + 1;
      size_t deleteCosts = matrix[i - 1][j] + 1;

      matrix[i][j] = std::min(std::min(replaceCosts, addCosts), deleteCosts);
    }
  }

  // Search the last row for the minimum value.
  size_t minDelta = delta + 1;
  for (size_t i = 0; i < m; ++i) {
    if (matrix[n - 1][i] < minDelta) {
      minDelta = matrix[n - 1][i];
    }
  }

  return minDelta;
}

// _____________________________________________________________________________
std::pair<std::vector<Entity>, size_t> QGramIndex::findMatches(
    const std::string& prefix) const {
  std::vector<Entity> matches;
  size_t numPedComputations = 0;

  std::string nPrefix = normalize(prefix);
  size_t delta = nPrefix.size() / 4;
  int threshold = nPrefix.size() - (_q * delta);

  if (nPrefix.size() > 0) {
    // Fetch all the inverted lists for each q-gram of the prefix.
    std::vector<const std::vector<size_t>*> lists;
    for (const std::string& qGram : computeQGrams(nPrefix)) {
      auto it = _invertedLists.find(qGram);
      if (it != _invertedLists.end()) {
        lists.push_back(&it->second);
      }
    }

    for (const std::pair<size_t, size_t>& pair : mergeLists(lists)) {
      size_t id = pair.first;
      size_t freq = pair.second;
      Entity entity = _entities[id - 1];  // ids are 1-based.

      // Compute the PED for all entities where comm(x,y) >= |x| - q * delta.
      if (static_cast<int>(freq) >= threshold) {
        // Compute the PED to the name of the entity.
        size_t ped = prefixEditDistance(nPrefix, normalize(entity.name), delta);
        numPedComputations++;

        if (ped <= delta) {
          entity.ped = ped;
          entity.matchedSynonym.clear();
          matches.push_back(entity);
          continue;
        }

        if (_withSynonyms) {
          // Compute the best matching synonym (the synonym with lowest PED).
          std::string bestMatchingSynonym;
          size_t bestPed = std::numeric_limits<std::size_t>::max();

          // Iterate through all synonyms and compute PED.
          for (std::string syn : entity.synonyms) {
            size_t synPed = prefixEditDistance(nPrefix, normalize(syn), delta);
            numPedComputations++;

            // Check if the synonym is the "best" matching synonym.
            if (synPed <= delta && synPed < bestPed) {
              bestPed = synPed;
              bestMatchingSynonym = syn;
            }
          }

          // Take the best matching synonym.
          if (bestMatchingSynonym.size() != 0) {
            entity.matchedSynonym = bestMatchingSynonym;
            entity.ped = bestPed;
            matches.push_back(entity);
          }
        }
      }
    }

    // Rank the matches.
    matches = rankMatches(matches);
  }

  return std::pair<std::vector<Entity>, size_t>(matches, numPedComputations);
}

// _____________________________________________________________________________
std::vector<Entity> QGramIndex::rankMatches(const std::vector<Entity>& list) {
  std::vector<Entity> result = list;
  std::sort(result.begin(), result.end(), _entityComparator);
  return result;
}

// _____________________________________________________________________________
std::vector<std::string> QGramIndex::computeQGrams(const std::string& word)
    const {
  std::vector<std::string> result;
  std::string padded = _padding + normalize(word);
  for (size_t i = 0; i < padded.size() - _q + 1; ++i) {
    result.push_back(padded.substr(i, _q));
  }

  return result;
}

// _____________________________________________________________________________
std::string QGramIndex::normalize(const std::string& str) {
  std::string s;
  for (size_t i = 0; i < str.size(); ++i) {
    if (!std::isalnum(str[i])) {
      continue;
    }
    s += static_cast<char>(tolower(str[i]));
  }

  return s;
}

// _____________________________________________________________________________
std::vector<std::string> QGramIndex::split(const std::string& text, char sep) {
  std::vector<std::string> tokens;
  std::size_t start = 0, end = 0;
  while ((end = text.find(sep, start)) != std::string::npos) {
    tokens.push_back(text.substr(start, end - start));
    start = end + 1;
  }
  if (start < end) {
    tokens.push_back(text.substr(start));
  }
  return tokens;
}
