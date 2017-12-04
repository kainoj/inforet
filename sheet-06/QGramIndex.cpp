// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Björn Buchhold <buchholb@cs.uni-freiburg.de>,
//          Patrick Brosi <brosi@cs.uni-freiburg.de>,
//          Claudius Korzen <korzen@cs.uni-freiburg.de>.

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "./QGramIndex.h"

// _____________________________________________________________________________
void QGramIndex::buildFromFile(const std::string& fileName) {
  _invertedLists.clear();

  std::string line;

  // Read the file line by line.
  std::ifstream in(fileName.c_str(), std::ios_base::in);
  size_t wordId = 0;
  // Ignore first line.
  std::getline(in, line);
  // Iterate through the remaining lines.
  while (std::getline(in, line)) {
    // size_t posTab1 = line.find('\t');
    // std::string word = line.substr(0, posTab1 + 1);
    //
    // size_t posTab2 = line.find('\t', posTab1+1);
    // size_t score = std::stoi(line.substr(posTab1+1, posTab2));
    // std::string desc = line.substr(posTab2+1, line.length());
    std::vector<std::string> parts = split(line, '\t');

    std::string name;
    if (parts.size() > 0) { name = parts[0]; }

    size_t score = 0;
    if (parts.size() > 1) { score = atoi(parts[1].c_str()); }

    std::string description;
    if (parts.size() > 2) { description = parts[2]; }

    std::string wikipediaUrl;
    if (parts.size() > 3) { wikipediaUrl = parts[3]; }

    entities.push_back(Entity{name, score, description, wikipediaUrl});

    wordId++;

    // Compute the q-grams of the (normalized) entity name.
    for (const std::string& qGram : computeQGrams(name)) {
      _invertedLists[qGram].push_back(wordId);
    }
  }
}

// _____________________________________________________________________________
std::vector<std::string> QGramIndex::computeQGrams(const std::string& word)
    const {
  std::vector<std::string> result;

  std::string padded = _padding + normalize(word);  //  + _padding;
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

std::vector<size_t> QGramIndex::mergeLists(std::vector<size_t> a,
                                           std::vector<size_t> b) {
  // TODO(przemek): implement O(n) merge
  a.insert(a.end(), b.begin(), b.end());
  std::sort(a.begin(), a.end());
  return a;
}

// _____________________________________________________________________________
size_t QGramIndex::prefixEditDistance(string x, string y, size_t delta) {
  size_t xs = x.size() + 1;
  size_t ys = std::min(y.size(), x.size() + delta + 1) + 1;
  std::vector<std::vector<size_t>> res(xs, std::vector<size_t>(ys, 0));

  // 1st row
  for (size_t j = 0; j < ys; j++)
    res[0][j] = j;

  // 1st col
  for (size_t i = 1; i < xs; i++)
    res[i][0] = i;

  for (size_t i = 1; i < xs; i++) {
    for (size_t j = 1; j < ys; j++) {
      if (x[i-1] == y[j-1])
        res[i][j] = res[i-1][j-1];
      else
        res[i][j] = 1 + min(min(res[i-1][j], res[i][j-1]), res[i-1][j-1]);
    }
  }
  size_t ped = *std::min_element(res[xs-1].begin(), res[xs-1].end());
  return ped <= delta ? ped : delta + 1;
}


// _____________________________________________________________________________
pair<vector<pair<Entity, size_t>>, size_t>
  QGramIndex::findMatches(string x, size_t delta) {
  vector<size_t> qgList = {};
  for (const std::string& qGram : computeQGrams(x)) {
    qgList = mergeLists(qgList, _invertedLists[qGram]);
  }

  std::vector<pair<size_t, size_t>> wordCount;
  if (qgList.size() > 0) {
    size_t cntr = 1;
    for (size_t i = 1; i < qgList.size(); i++) {
      if (qgList[i-1] == qgList[i]) {
        cntr++;
      } else {
        wordCount.push_back({qgList[i-1], cntr});
        cntr = 1;
      }
    }
    wordCount.push_back({qgList[qgList.size()-1], cntr});
  }

  pair<vector<pair<Entity, size_t>>, size_t> res;
  res.second = 0;

  for (auto w = wordCount.begin(); w != wordCount.end(); ++w) {
  //  std::cout << "w= " << entities[w->first-1] << '\n';
    if (static_cast<int>(w-> second)
          >= static_cast<int>(x.length() -  _q * delta)) {
      Entity entity = entities[w->first-1];
      size_t ped =  prefixEditDistance(
          QGramIndex::normalize(x),
          QGramIndex::normalize(entity.getName()), delta);
      res.second++;
      if (ped <= (size_t)delta)
         res.first.push_back({entity, ped});
    }
  }
//  std::cout << "---------------" << '\n';
  return res;
}

// _____________________________________________________________________________
// Helper function - comparing <pair<Entity, size_t>>
bool compareMatches(pair<Entity, size_t> m1, pair<Entity, size_t> m2) {
  if (m1.second < m2.second) return true;
  if (m1.second == m2.second) return m1.first.getScore() > m2.first.getScore();
  return false;
}

// _____________________________________________________________________________
vector<pair<Entity, size_t>>
  QGramIndex::rankMatches(vector<pair<Entity, size_t>> matches) {
  std::sort(matches.begin(), matches.end(), compareMatches);
  return matches;
}

// _____________________________________________________________________________
vector<pair<Entity, size_t>>
            QGramIndex::findAndRankMatches(string x, size_t delta) {
  auto matches = findMatches(x,  delta);
  return rankMatches(matches.first);
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
