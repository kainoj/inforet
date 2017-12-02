// Copyright 2017, University of Freiburg,
// Przemysław Joniak <prz.joniak@gmail.com>

#ifndef EVALUATEINVERTEDINDEX_H_
#define EVALUATEINVERTEDINDEX_H_

#include <unordered_map>
#include <string>
#include <vector>
#include <set>
#include <tuple>
#include "./InvertedIndex.h"

using std::string;
using std::vector;
using std::pair;
using std::unordered_map;
using std::set;
using std::tuple;

class EvaluateInvertedIndex {
 public:
  unordered_map<string, set<int>> read_benchmark(string file_name);
  double precision_at_k(vector<int> result_ids, set<int> relevant_ids, int k);
  double average_precision(vector<int> result_ids, set<int> relevant_ids);
  tuple<double, double, double> evaluate(
                                    InvertedIndex ii,
                                    unordered_map<string, set<int>> benchmark,
                                    bool use_refinements);
  unordered_map<string, set<int>> getBenchmark() const;
 private:
  unordered_map<string, set<int>> _benchmark;
};

#endif  // EVALUATEINVERTEDINDEX_H_
