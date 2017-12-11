// Copyright 2017, University of Freiburg
// Author: Przemyslaw Joniak <prz dot joniak at gmail dot com>

#include <gtest/gtest.h>
#include "./SearchServer.h"
#include "./QGramIndex.h"


// _____________________________________________________________________________
TEST(SearchServerTest, urlDecodeTest) {
  QGramIndex index(3, false);
  index.buildFromFile("a.txt");
  SearchServer s(index, 6666);
  ASSERT_EQ(L"the mätrix", s.urlDecode("the+m%C3%A4trix"));
  ASSERT_EQ(L"nirwana", s.urlDecode("nirwana"));
  ASSERT_EQ(L"Mikrösoft Windos", s.urlDecode("Mikr%C3%B6soft+Windos"));
  ASSERT_EQ(L"The hitschheiker guide", s.urlDecode("The+hitschheiker%20guide"));
}
