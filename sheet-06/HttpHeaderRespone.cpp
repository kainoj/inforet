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


// _____________________________________________________________________________
void HttpHeader::composeResponse200() {
  std::stringstream contentStream;
  std::ifstream file(fileName);
  if (file) {
    contentStream << file.rdbuf();
    file.close();
  }
  std::string body = contentStream.str();

  if (fileName == "search.html") {
    body = processQuerry(body);
  }

  response += "Content-Type: "   + getContentType() + "\r\n";
  response += "Content-Length: " + std::to_string(body.length() * sizeof(char));
  response += "\r\n\r\n";

  response += body;
}

// _____________________________________________________________________________
std::string HttpHeader::processQuerry(std::string content) {
  // No params given - remove %QUERY%
  size_t queryVarPos = content.find("%QUERY%");
  size_t resulVarPos = content.find("%RESULT%");

  if (params.find("q") == params.end()) {
    content.replace(queryVarPos, 7, "");
    content.replace(resulVarPos-7, 8, "");
    return content;
  }

  // Proces the querry and return results
  std::string query = params["q"];
  std::replace(query.begin(), query.end(), '+', ' ');
  std::cout << "Requested query:\t" << query<< '\n';
  content.replace(queryVarPos, 7, query);

  auto matches = Index.findAndRankMatches(query, query.length()/4);

  std::string res = "<table>";
  size_t bound = (matches.size() < 5) ? matches.size() : 5;
  for (size_t i = 0; i < bound; i++) {
    res += "<tr>";
    res += "<td class=\"result\">";
    res += "<a href=\"" + matches[i].first.getUrl() + "\">";
    res += matches[i].first.getName();
    res += "</a><td/>";
    res += "<td class=\"score\">" + std::to_string(matches[i].first.getScore());
    res += "<td/>";
    res += "<td class=\"desc\">" + matches[i].first.getTruncDesc()  + "<td/>";
    res += "</tr>";
    std::cout << "url: " <<  matches[i].first.getUrl() <<'\n';
    std::cout << "##" << matches[i].first.getDesc() << '\n';
  }
  res += "</table>";

  content.replace(content.find("%RESULT%"), 8, res);
  return content;
}
