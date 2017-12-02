// Copyright 2017, University of Freiburg
// Author: Przemyslaw Joniak,

#ifndef SERVER_H_
#define SERVER_H_

#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <string>

class Server {
 public:
  explicit Server(uint16_t port);
  void receive();

 private:
  uint16_t port;
  boost::asio::io_service ioService;
  // boost::asio::ip::tcp::endpoint server;
  boost::asio::ip::tcp::acceptor acceptor;
};

#endif  // SERVER_H_
