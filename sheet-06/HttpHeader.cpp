// Copyright 2017, University of Freiburg
// Authors: Przemyslaw Joniak <prz.joniak at gmail.com>

#include "./HttpHeader.h"

// _____________________________________________________________________________
HttpHeader::HttpHeader(std::string requestHeader, QGramIndex &Index)
  : header(requestHeader),
    Index(Index) {
  parseHttpHeader();
  parseParams();
  parseContentType();

  // Ignore leading '/'
  if (fileName.length()) fileName = fileName.substr(1, fileName.length()-1);

  status = composeStatusCode();
  composeResponse();
}

std::string HttpHeader::getNextWord(std::string str, std::string key) {
  size_t pos = str.find(key);
  if (pos == std::string::npos) {
    return "";
  }

  // Jump to word end
  pos = pos + key.length();

  // Ignore leading whitspace chars
  size_t spaces = 0;
  while ((pos + spaces) < str.length() && isspace(str[pos + spaces])) spaces++;
  pos+=spaces;

  // Get till next
  size_t i = 0;
  while ((pos + i) < str.length() && !isspace(str[pos + i])) i++;
  return std::string(str, pos, i);;
}


// _____________________________________________________________________________
void HttpHeader::parseHttpHeader() {
  method   = getNextWord(header, "");  // Gets 1st word (GET/POST/..)
  resource = getNextWord(header, method);
  host     = getNextWord(header, "Host: ");
  fileName = resource;                 // Suppose no parameters given
}


// _____________________________________________________________________________
void HttpHeader::parseParams() {
  params = {};

  size_t a;    // 'a' denotes idx of 1st char of param name
  size_t b;    // 'b' denotes idx of 1st char of param value
  size_t c;    // 'c' denotes idx of '&' char

  // Find first occurence of '?'
  a = resource.find('?');
  if (a == std::string::npos) return;  // Bye, no params.
  fileName = resource.substr(0, a);    // Truncate file name

  c = a;   // Consider '?' as first '&'

  do {
    a = c + 1;
    b = resource.find('=', a) + 1;
    if (b == std::string::npos) return;  // Upsi, invalid params were given

    c = resource.find('&', b);
    if (c == std::string::npos) c = resource.length();
    params[resource.substr(a, b-a-1)] = resource.substr(b, c-b);
  } while ( c != resource.length() );
}


// _____________________________________________________________________________
void HttpHeader::parseContentType() {
  int dotPos = fileName.length()-1;
    while (dotPos >= 0 && fileName[dotPos] != '.') dotPos--;
    std::string fileExtension(fileName, dotPos+1, fileName.length()-dotPos);
    if (contentTypes.find(fileExtension) == contentTypes.end())
      contentType = contentTypes.at("other");
    else
      contentType = contentTypes.at(fileExtension);
}

// _____________________________________________________________________________
size_t HttpHeader::composeStatusCode() {
  if (method != "GET") return 501;

  // Can be done better, eg realpath()
  if (fileName.find("../") != std::string::npos
     || fileName.find('/') != std::string::npos )  return 403;

  if (fileExists(fileName)) return 200;
  else
     return 404;
  return 500;
}

// _____________________________________________________________________________
bool HttpHeader::fileExists(const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

// _____________________________________________________________________________

void HttpHeader::printBriefInfo() {
  std::cout << method << "\t" << fileName << "\n";
  std::cout << "Host:\t"   << host   << "\n";
  std::cout << "Status:\t" << status << "\n\n";
}


std::string HttpHeader::getResponse() const {
  return response;
}


// _____________________________________________________________________________
// Getters, for testing
std::string HttpHeader::getMethod() const {
  return method;
}

std::string HttpHeader::getFileName() const {
  return fileName;
}

std::string HttpHeader::getHost() const {
  return host;
}

std::unordered_map<std::string, std::string> HttpHeader::getParams() const {
  return params;
}

std::string HttpHeader::getContentType() const {
  return contentType;
}

size_t HttpHeader::getStatusCode() const {
  return status;
}
