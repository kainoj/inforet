
// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Claudius Korzen <korzen@cs.uni-freiburg.de>.

#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <string>

#include "./QGramIndex.h"
#include "./SearchServer.h"

// A simple server that handles fuzzy prefix search requests and file requests.
int main(int argc, char** argv) {
  // Parse the command line arguments.
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <file> <port> [--with-synonyms]";
    std::cerr << std::endl;
    exit(1);
  }
  std::string fileName = argv[1];
  uint16_t port = atoi(argv[2]);
  bool withSynonyms = false;
  if (argc > 3) {
    std::string withSynonymsStr = argv[3];
    withSynonyms = withSynonymsStr.compare("--with-synonyms") == 0;
  }

  // Build the q-gram index.
  std::cout << "Building the q-gram index from '" << fileName << "' ... ";
  std::cout << std::flush;
  QGramIndex index(3, withSynonyms);
  index.buildFromFile(fileName);
  std::cout << "Done!" << std::endl;

  // Start the server loop.
  std::cout << "Starting the server on port '" << port << "' ... ";
  SearchServer server(index, port);
  std::cout << "Done!" << std::endl;

  server.run();
}
