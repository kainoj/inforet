// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Björn Buchhold <buchhold@cs.uni-freiburg.de>,
// Patrick Brosi <brosi@cs.uni-freiburg.de>.

#include <iostream>
#include <string>
#include <fstream>
#include <tuple>

#include "./InvertedIndex.h"
#include "./EvaluateInvertedIndex.h"

// _____________________________________________________________________________
int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage ./InvertedIndexMain <dataset file> <benchmark file>\n";
    exit(1);
  }

  string fileName = argv[1];
  string benchName = argv[2];
  string querry;

  InvertedIndex ii;
  ii.readFromFile(fileName, 0.12, 1.0);

  EvaluateInvertedIndex eii;
  eii.read_benchmark(benchName);

  auto evaluated = eii.evaluate(ii, eii.getBenchmark(), false);

  std::cout << "MP@3 = " << std::get<0>(evaluated) << '\n';
  std::cout << "MP@R = " << std::get<1>(evaluated) << '\n';
  std::cout << "MAP  = " << std::get<2>(evaluated) << '\n';

  return 0;
}
