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
#include "./Server.h"

// _____________________________________________________________________________
int main(int argc, char** argv) {
  // Parse command line arguments.
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << "<entity-file> <port>\n";
    exit(1);
  }
  std::string fileName = argv[1];
  uint16_t port = atoi(argv[2]);
  std::string query;

  std::cout << "Building index from '" << fileName << "' ... ";
  std::cout << std::flush;

  QGramIndex index(3);
  index.buildFromFile(fileName);
  std::cout << "done!" << "\n\n";

  std::cout << "Running server on port: " << port <<  " ... \n";
  Server server(port);

  while (true) {
    server.receive();
  }

  return 0;
}
