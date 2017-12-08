// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Claudius Korzen <korzen@cs.uni-freiburg.de>.

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>

#include "./SearchServer.h"


// _____________________________________________________________________________
void SearchServer::run() {
  // The main server loop.
  while (true) {
    // Wait for the client.
    std::cout << "Waiting on port " << _server.port() << " ... " << std::flush;
    _acceptor.accept(_client);
    std::cout << "client connected from "
        << _client.remote_endpoint().address().to_string() << std::endl;

    try {
      // Set the timeout for the client.
      _timer.expires_from_now(boost::posix_time::seconds(1));
      _timer.async_wait(boost::bind(&SearchServer::handleTimeout, this, _1));

      // Read only the first line of the request from the client.
      boost::asio::async_read_until(_client, _requestBuffer,
          DEFAULT_LINE_DELIMITER,
          boost::bind(&SearchServer::handleRequest, this, _1));

      _ioService.run();
      _ioService.reset();
    } catch (boost::exception& e) {
      // Socket was close by remote, continue listening
      std::cout << "WARN: " << boost::diagnostic_information(e) << std::endl;
      continue;
    }
  }
}

// _____________________________________________________________________________
void SearchServer::handleTimeout(const boost::system::error_code& e) {
  if (e) {
    return;
  }

  std::cout << "Timeout. " << std::endl;

  // Clear the request buffer.
  _requestBuffer.consume(_requestBuffer.size());

  // Cancel the timeout timer.
  _timer.cancel();

  // Close the connection to the client.
  _client.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
  _client.close();
}

// _____________________________________________________________________________
void SearchServer::handleRequest(const boost::system::error_code& e) {
  if (e) {
    return;
  }

  std::string request;
  std::istream istream(&_requestBuffer);
  std::getline(istream, request);

  // IMPORTANT NOTE: read_until() seems to actually read more than up to the
  // delimiter '\r\n'. So check if the reqString still contains the delimiter
  // and remove it.
  size_t pos1 = request.find("\r\n");
  if (pos1 != std::string::npos) {
    request = request.replace(pos1, 2, "");
  }
  size_t pos2 = request.find("\r");
  if (pos2 != std::string::npos) {
    request = request.replace(pos2, 1, "");
  }

  // Handle the request line and create the response.
  std::stringstream response = createResponse(request);

  // Send the response to the client.
  boost::asio::write(_client, boost::asio::buffer(response.str()),
      boost::asio::transfer_all());

  // Clear the request buffer.
  _requestBuffer.consume(_requestBuffer.size());

  // Cancel the timeout timer.
  _timer.cancel();

  // Close the connection to the client.
  _client.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
  _client.close();
}

// _____________________________________________________________________________
std::stringstream SearchServer::createResponse(const std::string& requestLine)
    const {
  std::stringstream response;

  std::smatch matcher;
  if (!std::regex_search(requestLine, matcher, HTTP_REQUEST_HEADER_REGEX)) {
    // The request contains no valid HTTP header.
    std::cout << "RESPONSE: 405 (No valid HTTP request)." << std::endl;
    response << HTTP_ERROR_RESPONSES.find(405)->second;
    return response;
  }

  // Obtain the HTTP method, the request fileName and the parameters.
  std::string httpMethod = matcher[1];
  std::string fileName = matcher[2];
  std::string parameters = matcher[3];

  std::cout << "REQUEST: HTTP_METHOD: " << httpMethod << ", FILE_NAME: "
            << fileName << ", PARAMETERS: " << parameters << std::endl;

  if (httpMethod.compare("GET") != 0) {
    // Only GET requests are supported.
    std::cout << "RESPONSE: 405 (No GET request)." << std::endl;
    response << HTTP_ERROR_RESPONSES.find(405)->second;
    return response;
  }

  if (fileName.length() == 0) {
    // No fileName is given, take the default one.
    std::cout << "Setting fileName to default '"<< DEFAULT_FILE_NAME << "'."
              << std::endl;
    fileName = DEFAULT_FILE_NAME;
  }

  if (!std::regex_match(fileName, std::regex("[a-zA-Z.]+"))) {
    // The fileName contains invalid characters.
    std::cout << "RESPONSE: 403 (fileName with invalid chars)." << std::endl;
    response << HTTP_ERROR_RESPONSES.find(403)->second;
    return response;
  }

  std::ifstream fstream((SERVE_DIR + fileName).c_str(), std::ios_base::in);
  if (!fstream) {
    // The requested file does not exist or is not readable.
    std::cout << "RESPONSE: 404 (File does not exist)." << std::endl;
        response << HTTP_ERROR_RESPONSES.find(404)->second;
    return response;
  }

  // Read the requested file.
  std::stringstream ss;
  ss << fstream.rdbuf();

  if (fileName.compare("search.html") == 0) {
    // Handle a fuzzy prefix search request.
    std::cout << "Handling fuzzy prefix search request." << std::endl;
    ss = handleFuzzyPrefixSearchRequest(parameters, ss);
  }

  // Create the HTTP response.
  std::cout << "RESPONSE: 200 (OK)." << std::endl;
  std::string contentType = getContentType(fileName);
  response << HTTP_OK_HEADER << DEFAULT_LINE_DELIMITER
           << "Content-type: " << contentType << DEFAULT_LINE_DELIMITER
           << "Content-length: " << ss.str().size() << DEFAULT_LINE_DELIMITER
           << DEFAULT_LINE_DELIMITER
           << ss.str();

  return response;
}

// _____________________________________________________________________________
std::stringstream SearchServer::handleFuzzyPrefixSearchRequest(
    const std::string& params, const std::stringstream& stream) const {
  // Check if there is a query given in the parameters.
  std::string query = "";
  size_t pos = params.find("?q=");
  if (pos != std::string::npos) {
    query = params.substr(pos + 3);
  }

  // Decode all url-encoded whitespaces.
  std::replace(query.begin(), query.end(), '+', ' ');

  size_t numResultsTotal = 0;
  size_t numResultsToShow = 0;

  std::string searchHtml = stream.str();

  // Pass the query to the q-gram index and create the related HTML fragment.
  std::stringstream resultHtml;
  if (query.length() != 0) {
    std::pair<std::vector<Entity>, size_t> result = _index.findMatches(query);
    std::vector<Entity> matches = result.first;
    numResultsTotal = matches.size();
    numResultsToShow = std::min(matches.size(), NUM_SEARCH_RESULTS_TO_SHOW);
    for (size_t i = 0; i < numResultsToShow; ++i) {
      resultHtml << translateToHtml(matches[i]) << DEFAULT_LINE_DELIMITER;
    }
  }

  // Plug in the query into the search.html.
  replaceAll(searchHtml, "%QUERY%", query);

  // Plug in the result header.
  std::stringstream resultHeader;
  if (numResultsToShow == 0) {
    resultHeader << "Nothing to show.";
  } else {
    resultHeader << "Found " << numResultsTotal << " results for query '"
        << query << "'.";
  }
  replaceAll(searchHtml, "%RESULT_HEADER%", resultHeader.str());

  // Plug in the HTML fragment containing the search results.
  replaceAll(searchHtml, "%RESULT%", resultHtml.str());

  return std::stringstream(searchHtml);
}

// _____________________________________________________________________________
std::string SearchServer::translateToHtml(const Entity& entity) const {
  std::string html = std::string(_entityHtmlPattern);

  // Plug in the name of the entity into the HTML pattern.
  std::string name = entity.name;
  replaceAll(html, "%ENTITY_NAME%", name);

  // Plug in the matched synonym (if any).
  std::string synonym;
  if (entity.matchedSynonym.length() != 0) {
    synonym = "synonym: " + entity.matchedSynonym;
  }
  replaceAll(html, "%ENTITY_SYNONYM%", synonym);

  // Plug in the description of the entity.
  std::string description = entity.description;
  replaceAll(html, "%ENTITY_DESCRIPTION%", description);

  // Plug in the image url of the entity.
  std::string imageUrl = entity.imageUrl;
  if (imageUrl.length() == 0) {
    // No image url is given, take the default one.
    imageUrl = DEFAULT_ENTITY_IMAGE_URL;
  }
  replaceAll(html, "%ENTITY_IMAGE%", imageUrl);

  // Plug in the URL to the related Wikidata page (if any).
  std::stringstream wikidataUrl;
  if (entity.wikidataId.length() != 0) {
    wikidataUrl << "https://www.wikidata.org/wiki/" << entity.wikidataId;
  }
  replaceAll(html, "%ENTITY_WIKIDATA_URL%", wikidataUrl.str());

  // Plug in the URL to the related Wikipedia page (if any).
  std::string wikipediaUrl;
  if (entity.wikipediaUrl.length() != 0) {
    wikipediaUrl = entity.wikipediaUrl;
  }
  replaceAll(html, "%ENTITY_WIKIPEDIA_URL%", wikipediaUrl);

  return html;
}

// _____________________________________________________________________________
std::string SearchServer::getContentType(const std::string& fileName) const {
  std::string contentType = DEFAULT_CONTENT_TYPE;

  // Obtain the file extension of the given file.
  std::string fileExtension = "";
  size_t pos = fileName.find_last_of(".");
  if (pos != std::string::npos) {
    fileExtension = fileName.substr(pos);
  }

  // Look up the content type for the file extension.
  if (CONTENT_TYPES.find(fileExtension) != CONTENT_TYPES.end()) {
    contentType = CONTENT_TYPES.find(fileExtension)->second;
  }

  return contentType;
}

// _____________________________________________________________________________
void SearchServer::replaceAll(std::string& str, const std::string& from,
    const std::string& to) const {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
}

