// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Björn Buchhold <buchholb@cs.uni-freiburg.de>,
//          Patrick Brosi <brosi@cs.uni-freiburg.de>,
//          Claudius Korzen <korzen@cs.uni-freiburg.de>.

#include <stdio.h>
#include <iostream>
#include <string>
#include <chrono>

#include "./QGramIndex.h"

// _____________________________________________________________________________
int main(int argc, char** argv) {
  // Parse command line arguments.
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <entity-file>\n";
    exit(1);
  }
  std::string fileName = argv[1];
  std::string query;

  std::cout << "Building index from '" << fileName << "' ...";
  std::cout << std::flush;

  QGramIndex index(3);
  index.buildFromFile(fileName);
  std::cout << "done!" << "\n\n";

  while (1) {
    std::cout << "_> ";
    std::getline(std::cin, query);
    query = QGramIndex::normalize(query);

    auto start = std::chrono::high_resolution_clock::now();
    auto mat = index.findMatches(query, query.size()/4);
    auto res = index.rankMatches(mat.first);
    auto end = std::chrono::high_resolution_clock::now();
    auto query_time = std::chrono::duration_cast<std::chrono::microseconds>
                   (end - start).count()/1000;

    size_t no = res.size() < 5? res.size() : 5;
    std::cout << "  Showing " << no << " out of " << res.size() << " results. ["
              << query_time << " ms]" << std::endl;
    std::cout << "\n/*\n\tTime: \t" << query_time << '\n'
              << "\t#PED: \t" << mat.second << '\n'
              << "\t#RES: \t" << res.size() << "\n*/\n\n";
    for (size_t i = 0; i < no; i++) {
      std::cout << "> " << res[i].first.getName()
                << '\t' << res[i].first.getDesc() << '\n';
      std::cout << "\n";
    }
    std::cout << '\n';
  }
  return 0;
}
