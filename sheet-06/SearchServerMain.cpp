// Copyright 2017, University of Freiburg,
// Author: Przemyslaw Joniak

#include <stdio.h>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

#include "./HttpHeader.h"

// _____________________________________________________________________________
int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << "<entity-file> <port>\n";
    exit(1);
  }
  std::string fileName = argv[1];
  uint16_t port = atoi(argv[2]);

  std::cout << "Building index from '" << fileName << "' ... " << std::flush;

  QGramIndex Index(3);
  Index.buildFromFile(fileName);
  std::cout << "done!" << "\n\n"<< "Running server on port: " << port <<  "\n";

  boost::asio::io_service ioService;
  boost::asio::ip::tcp::endpoint server(boost::asio::ip::tcp::v4(), port);
  boost::asio::ip::tcp::acceptor acceptor(ioService, server);

  while (true) {
    std::cout << "Waiting for query on port " << port << " ... " << std::flush;
    boost::asio::ip::tcp::socket client(ioService);
    acceptor.accept(client);
    std::cout << "client connected from "
              << client.remote_endpoint().address().to_string() << std::endl;

    boost::asio::streambuf requestBuffer;
    boost::asio::read_until(client, requestBuffer, "\r\n");
    std::istream istream(&requestBuffer);

    std::string hdrstr(std::istreambuf_iterator<char>(istream), {});

    HttpHeader header(hdrstr, Index);
    header.printBriefInfo();

    boost::asio::write(client, boost::asio::buffer(header.getResponse()),
      boost::asio::transfer_all());
  }
  return 0;
}
