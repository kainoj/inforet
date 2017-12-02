// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Björn Buchhold <buchhold@cs.uni-freiburg.de>,
// Patrick Brosi <brosi@cs.uni-freiburg.de>.

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <math.h>
#include "./InvertedIndex.h"

// Define a infinity threshold
#define INF std::numeric_limits<double>::max()-42.0

// _____________________________________________________________________________
void InvertedIndex::readFromFile(const string& fileName, double b, double k) {
  string line;
  std::ifstream in(fileName.c_str());
  uint64_t recordId = 0;
  // _docLen is indexed from 1!
  _docLen.push_back(0);
  _avgDocLen = 0.0;

  // 1st pass: compute tf, docs len & avg doc len
  while (std::getline(in, line)) {
    recordId++;
    _docLen.push_back(0);
    size_t i = 0;
    size_t start = 0;
    while (i <= line.size()) {
      // The > 0 so that we don't split at non-ascii chars.
      // It is very important that one can search for "björn" :-).
      if (i == line.size() || (line[i] > 0 && !isalnum(line[i])))  {
        if (i > start) {
         string word = getLowercase(line.substr(start, i - start));
         _docLen[recordId]++;
          // Automatically default constructs a list if the
          // word is seen for the first time.
          if (_invertedLists[word].empty()
            || _invertedLists[word].back().first != recordId)
            _invertedLists[word].push_back({recordId, 0.0});
          _invertedLists[word].back().second+=1.0;
        }
        start = i + 1;
      }
      ++i;
    }
    // Update avg doc len of recordId-th doc
    _avgDocLen+=_docLen[recordId];
  }
  // Compute avg len (-1, because of 0th padding)
  const double N = _docLen.size()-1.0;
  _avgDocLen/=N;

  // 2nd pass: compute BM25
  for (auto it = _invertedLists.begin(); it != _invertedLists.end(); ++it) {
    double idf =  log2(N/it->second.size());
    for (auto v = it->second.begin();
             v != it->second.end();
          ++v) {
       // Compute BM25 score
       double DL = _docLen[v->first]*1.0;
       double tf = v->second*1.0;
       double alpha = 1.0 - b + b*(DL/_avgDocLen);

       // Fist approach: fails when k → inf ( = nan)
       // v->second = tf*(k+1)/(k*alpha+tf)*idf;

       // Second approach: doesn't fail when  k → inf (but fails when k→0)
       // v->second = tf * (1.0 + 1.0/k) / (alpha + tf/k) * idf;

       // Third approach: if k is really big (excedes inf threshold), use 2nd
       // formula, otherwise, use 1st

       if ( k >= INF ) {
         v->second = tf * 1.0 / (alpha) * idf;
       } else {
         v->second = tf*(k+1)/(k*alpha+tf)*idf;
       }
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
std::ostream& operator<<(std::ostream& os, const std::vector<Posting> v) {
  for (auto it = v.begin(); it != v.end(); ++it) {
    os << "(" << it->first << ", " << it->second << ") ";
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
std::vector<Posting> InvertedIndex::merge(std::vector<Posting> v1,
                                          std::vector<Posting> v2) {
  std::vector<Posting> res;
  size_t i = 0;  // Current index in v1
  size_t j = 0;  // Current index in v2
  while ( i < v1.size() && j < v2.size() ) {
    if (v1[i].first == v2[j].first) {
      res.push_back({v1[i].first, v1[i].second + v2[j].second});
      i++;
      j++;
    } else {
      if (v1[i].first < v2[j].first) {
        res.push_back(v1[i]);
        i++;
      } else {
        res.push_back(v2[j]);
        j++;
      }
    }
  }
  // while() loop has ended because end of at least one vector {v1, v2} was
  // reachead. Thus, at most only one of below fors will be executed.
  for (; i < v1.size(); i++) res.push_back(v1[i]);
  for (; j < v2.size(); j++) res.push_back(v2[j]);
  return res;
}

// Helper function - compares Postings in descending order.
bool postingCompDesc(Posting a, Posting b) {
  return (a.second > b.second);
}

// ____________________________________________________________________________
std::vector<Posting> InvertedIndex::process_querry(string querry,
                                                   bool use_refinements) {
  if (use_refinements)
    std::cout << "Ups, use_refinements not implemented.\n";

  std::vector<Posting> querry_res;
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
         querry_res = merge(querry_res, _invertedLists[word]);
       }
      }
      start = i + 1;
    }
    ++i;
  }
  std::sort(querry_res.begin(), querry_res.end(), postingCompDesc);
  if (querry_res.size() < 3)
    return querry_res;
  std::vector<Posting> top3res;
  top3res.push_back(querry_res[0]);
  top3res.push_back(querry_res[1]);
  top3res.push_back(querry_res[2]);
  return top3res;
}
