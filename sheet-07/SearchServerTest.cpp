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

  // std::string m = "ä";
  // std::cout << "m.length = " << m.length() << "\n";   // = 2
  // std::cout << "sizeof(m) =" <<  sizeof(m) << '\n';  // = 32
  //
  // std::wstring wm = L"ä";
  // std::cout << "wm.length = " << wm.length() << "\n";   // = 1
  // std::cout << "sizeof(wm) =" <<  sizeof(wm) << '\n';  // = 32


  std::cout << "len =" << '\n';
  ASSERT_EQ("the mätrix", s.urlDecode("the+m%C3%A4trix"));
  ASSERT_EQ("nirwana", s.urlDecode("nirwana"));
  ASSERT_EQ("Mikrösoft Windos", s.urlDecode("Mikr%C3%B6soft+Windos"));
  ASSERT_EQ("The hitschheiker guide", s.urlDecode("The+hitschheiker%20guide"));
}
