// Copyright 2017, University of Freiburg
// Authors: Przemyslaw Joniak <prz.joniak at gmail.com>

#include "./HttpHeader.h"

// _____________________________________________________________________________
void HttpHeader::composeResponse() {
  response = "HTTP/1.1 " + statusCodes.at(status) + "\r\n";
  response += "Server: kainoj2k17\r\n";
  switch (status) {
    case 200:
      composeResponse200();
      break;
    // case 300, 500... etc
    default:
      composeResponse400();
    break;
  }
}

// _____________________________________________________________________________
void HttpHeader::composeResponse400() {
  std::string body = "Error: " + statusCodes.at(status);
  response += "Content-Type: " + contentType + "\r\n";
  response += "Content-Length: ";
  response += std::to_string(body.length()*sizeof(char));
  response += "\r\n\r\n";
  response += body;
  response += "\r\n\r\n";
}

void HttpHeader::composeResponse200() {
  std::stringstream contentStream;
  std::ifstream file(fileName);
  if (file) {
    contentStream << file.rdbuf();
    file.close();
  }
  std::string body = contentStream.str();

  response += "Content-Type: "   + getContentType() + "\r\n";
  response += "Content-Length: " + std::to_string(body.length() * sizeof(char));
  response += "\r\n\r\n";

  response += body;
}
