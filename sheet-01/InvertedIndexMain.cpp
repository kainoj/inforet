// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Björn Buchhold <buchhold@cs.uni-freiburg.de>,
// Patrick Brosi <brosi@cs.uni-freiburg.de>.

#include <iostream>
#include <string>
#include <fstream>
#include "./InvertedIndex.h"

// _____________________________________________________________________________
int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage ./InvertedIndexMain <file>\n";
    exit(1);
  }

  string fileName = argv[1];
  string querry;

  InvertedIndex ii;
  ii.readFromFile(fileName);

  // Load whole file to memory to prevent from multiple
  // i/o actions while querying (good for small files only)
  std::vector<string> records;
  std::ifstream in(fileName.c_str());
  string line;
  while (std::getline(in, line)) {
    records.push_back(line);
  }

  while (1) {
    std::cout << "> ";
    std::getline(std::cin, querry);
    std::vector<uint64_t> qry_res = ii.process_querry(querry);

    std::cout << "> Found " << qry_res.size() << " results. Showing ";
    std::cout << (qry_res.size() < 3 ? qry_res.size() : 3) << " of them:\n\n";

    for (size_t i = 0; i < 3 && i < qry_res.size(); i++) {
      // qry_res[i]-1 because records are indexed from 1
      std::cout << records[qry_res[i]-1] << " ";
      std::cout << "\n------------------------------------------------------\n";
    }
    std::cout << std::endl;
  }
}
