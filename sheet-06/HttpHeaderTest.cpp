// Copyright 2017, University of Freiburg
// Author: Przemyslaw Joniak <prz dot joniak at gmail dot com>

#include <gtest/gtest.h>
#include "./HttpHeader.h"
#include "./QGramIndex.h"

QGramIndex Index(1);

// _____________________________________________________________________________
TEST(HttpHeaderTest, testExtractSubstring) {
  HttpHeader header("", Index);
  std::string str("aa b  \r  \t   \rccc\r\nd\r   eeee");

  ASSERT_EQ("aa",  header.getNextWord(str, ""));
  ASSERT_EQ("b",   header.getNextWord(str, "aa"));
  ASSERT_EQ("ccc", header.getNextWord(str, "b"));
  ASSERT_EQ("d",   header.getNextWord(str, "ccc"));
  ASSERT_EQ("eeee", header.getNextWord(str, "d"));

  std::string str2("aa b  \r  \t   \rccc\r\nd\r   eeee   \t\r\n");
  ASSERT_EQ("", header.getNextWord(str2, "eeee"));
}

// _____________________________________________________________________________
TEST(HttpHeaderTest, testParseHttpHeader) {
  HttpHeader header("GET /awesomeresource HTTP/.1\r\nHost: ex.com\r\n", Index);
  ASSERT_EQ("GET",              header.getMethod());
  ASSERT_EQ("awesomeresource", header.getFileName());
  ASSERT_EQ("ex.com",      header.getHost());


  HttpHeader header2("GET /res?p1=v1&p2=v2 HTTP/.1\r\nHost: ex.com\r\n", Index);
  ASSERT_EQ("GET",         header2.getMethod());
  ASSERT_EQ("res",        header2.getFileName());
  ASSERT_EQ("ex.com", header2.getHost());
  ASSERT_EQ("v1", header2.getParams()["p1"]);
  ASSERT_EQ("v2", header2.getParams()["p2"]);
}

// _____________________________________________________________________________
TEST(HttpHeaderTest, testParseContentType) {
  HttpHeader h1("GET /a.html HTTP/.1\r\nHost: example.com\r\n", Index);
  ASSERT_EQ("text/html", h1.getContentType());

  HttpHeader h2("GET /b.css HTTP/.1\r\nHost: example.com\r\n", Index);
  ASSERT_EQ("text/css", h2.getContentType());

  HttpHeader h3("GET /c.txt HTTP/.1\r\nHost: example.com\r\n", Index);
  ASSERT_EQ("text/plain", h3.getContentType());

  HttpHeader h4("GET /c.asd HTTP/.1\r\nHost: example.com\r\n", Index);
  ASSERT_EQ("application/octet-stream", h4.getContentType());

  HttpHeader h5("GET /ddd HTTP/.1\r\nHost: example.com\r\n", Index);
  ASSERT_EQ("application/octet-stream", h4.getContentType());
}

// TEST(HttpHeaderTest, testComposeStatusCode) {
//   HttpHeader h1("GET /a.html HTTP/.1\r\nHost: example.com\r\n");
//   ASSERT_EQ("text/html", h1.getContentType());
//
// }
