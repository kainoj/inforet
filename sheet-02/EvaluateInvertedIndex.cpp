// Copyright 2017, University of Freiburg,
// Przemysław Joniak <prz.joniak@gmail.com>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

#include "./EvaluateInvertedIndex.h"

// _____________________________________________________________________________
unordered_map<string, set<int>>
               EvaluateInvertedIndex::read_benchmark(string file_name) {
  string line;
  std::ifstream in(file_name.c_str());
  while (std::getline(in, line)) {
    size_t i = 0;
    while (i < line.length() && line[i] != '\t') i++;
    if (i == line.length())
      std::cout << "Warrning: incorrect record format (TAB not found)";
    std::vector<int> ids;
    std::istringstream is(line.substr(i+1, line.length()));
    ids.assign(std::istream_iterator<int>(is), std::istream_iterator<int>());
    _benchmark.insert({line.substr(0, i),
                       std::set<int>(ids.begin(), ids.end())});
  }
  return _benchmark;
}

// _____________________________________________________________________________
double EvaluateInvertedIndex::precision_at_k(vector<int> result_ids,
                                             set<int> relevant_ids,
                                             int k) {
  int cntr = 0;
  for (size_t i = 0; i < (size_t)k && i < result_ids.size(); i++) {
    if (relevant_ids.find(result_ids[i]) != relevant_ids.end()) cntr++;
  }
  return 1.0*cntr/(1.0*k);
}

// _____________________________________________________________________________
double EvaluateInvertedIndex::average_precision(vector<int> result_ids,
                                                set<int> relevant_ids) {
  double ap = 0;
  size_t relcntr = 0;  // Counter of relevanat docs so far
  for (size_t i = 0;
       i < result_ids.size() && relcntr < relevant_ids.size();
       i++) {
    if (relevant_ids.find(result_ids[i]) != relevant_ids.end()) {
      relcntr++;
      ap += relcntr/(i+1.0);
    }
  }
  return ap / (relevant_ids.size()*1.0);
}

// _____________________________________________________________________________
tuple<double, double, double> EvaluateInvertedIndex::evaluate(
                                  InvertedIndex ii,
                                  unordered_map<string, set<int>> benchmark,
                                  bool use_refinements
                                ) {
  double MP3 = 0;
  double MPR = 0;
  double MAP = 0;

  EvaluateInvertedIndex eii;
  for (auto qry = benchmark.begin(); qry != benchmark.end(); ++qry) {
    // One has to fetch vectors of ids form vector of pairs
    std::vector<Posting> posting = ii.process_querry(qry->first, false);
    std::vector<int> res_ids;
    for (auto it = std::make_move_iterator(posting.begin()),
             end = std::make_move_iterator(posting.end());
             it != end; ++it) {
      res_ids.push_back(std::move(it->first));
    }
    MP3 += eii.precision_at_k(res_ids, qry->second, 3);
    MPR += eii.precision_at_k(res_ids, qry->second, qry->second.size());
    MAP += eii.average_precision(res_ids, qry->second);
  }

  MP3 = MP3/(1.0*benchmark.size());
  MPR = MPR/(1.0*benchmark.size());
  MAP = MAP/(1.0*benchmark.size());

  return std::make_tuple(MP3, MPR, MAP);
}


// _____________________________________________________________________________
unordered_map<string, set<int>> EvaluateInvertedIndex::getBenchmark() const {
  return _benchmark;
}
