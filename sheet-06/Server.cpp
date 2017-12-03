// Copyright 2017, University of Freiburg
// Authors: Przemyslaw Joniak <prz.joniak at gmail.com>

#include "./Server.h"
#include "./HttpHeader.h"

// _____________________________________________________________________________
Server::Server(uint16_t port) :
     port(port),
     acceptor(ioService,
             boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
}

// _____________________________________________________________________________
void Server::receive() {
  std::cout << "Waiting for query on port " << port << " ... " << std::flush;
    boost::asio::ip::tcp::socket client(ioService);
    acceptor.accept(client);
    std::cout << "client connected from "
        << client.remote_endpoint().address().to_string() << std::endl;

    boost::asio::streambuf requestBuffer;
    boost::asio::read_until(client, requestBuffer, "\r\n");
    std::istream istream(&requestBuffer);

    std::string hdrstr(std::istreambuf_iterator<char>(istream), {});

    HttpHeader header(hdrstr);
    header.printBriefInfo();

    boost::asio::write(client, boost::asio::buffer(header.getResponse()),
      boost::asio::transfer_all());

    // boost::asio::write(client, boost::asio::buffer(contentStream.str()),
    //   boost::asio::transfer_all());
}
