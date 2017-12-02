// Copyright 2017, University of Freiburg
// Author: Przemyslaw Joniak,

#ifndef HTTP_HEADER_H_
#define HTTP_HEADER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <sys/stat.h>

class HttpHeader {
 public:
  explicit HttpHeader(std::string requestHeader);

  // Finds string `key` in `str` and returns the next word
  // (whitspace separated, multile whitespaces are ignored)
  std::string getNextWord(std::string str, std::string key);

  void printBriefInfo();
  std::string getResponse() const;

  // Getters - for testing purposes
  std::string getMethod() const;
  std::string getFileName() const;
  std::string getHost() const;
  std::unordered_map<std::string, std::string> getParams() const;
  std::string getContentType() const;
  std::size_t getStatusCode() const;

 private:
  // Request header
  std::string header;
  std::string fileName;
  std::string method;
  std::string host;
  std::string resource;  // resource := fileName + (optional)params
  std::unordered_map<std::string, std::string> params;

  // Response headers
  std::size_t status;
  std::string contentType;
  std::string response;

  void parseHttpHeader();
  void parseParams();
  void parseContentType();

  size_t composeStatusCode();
  void composeResponse();
  void composeResponse200();
  void composeResponse300();
  void composeResponse400();  // >400


  bool fileExists(const std::string& name);

  std::unordered_map<std::string, std::string> contentTypes {
    {"txt",   "text/plain"},
    {"html",  "text/html"},
    {"css",   "text/css"},
    {"jpg",   "image/jpeg"},
    {"jpeg",  "image/jpeg"},
    {"png",   "image/png"},
    {"pdf",   "application/pdf"},
    {"other", "application/octet-stream"}};

  const std::unordered_map<int, std::string> statusCodes {
    {200, "200 OK"},
    {301, "301 Moved Permanently"},
    {403, "403 Forbidden"},
    {404, "404 Not Found"},
    {500, "500 Internal Server Error"},
    {501, "501 Not Implemented"}};
};

#endif  // HTTP_HEADER_H_
