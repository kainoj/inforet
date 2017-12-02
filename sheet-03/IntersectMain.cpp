// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@cs.uni-freiburg.de>,
//          Claudius Korzen <korzen@cs.uni-freiburg.de>.

#include <stddef.h>
#include <chrono>
#include <iostream>
#include <vector>
#include "math.h"
#include "./PostingList.h"

// _____________________________________________________________________________
int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage " << argv[0] << " <posting lists>\n";
    exit(1);
  }

  size_t numLists = argc - 1;

  // Read the posting lists.
  std::vector<PostingList> lists;
  for (size_t i = 0; i < numLists; i++) {
    std::cout << "Reading list '" << argv[i + 1] << "'\t...\t";
    PostingList list;
    list.readFromFile(argv[i + 1]);
    list.addPosting(INF, 42);
    lists.push_back(list);
    std::cout << "Done. Size: " << lists[i].size() << "." << std::endl;
  }

  // Intersect the lists pairwise.
  std::cout << std::endl;
  for (size_t i = 0; i < numLists; i++) {
    for (size_t j = 0; j < i; j++) {
      std::cout << "\n> Intersect '" << argv[i + 1] << "' & '" << argv[j + 1]
                << "'." << std::endl;

      // Intersect lists[i] and lists[j] using the baseline and my algorithm
      size_t timeBaseline = 0;
      size_t sizeBaseline = 0;
      size_t timeMy = 0;
      size_t sizeMy = 0;

      for (size_t r = 0; r < 5; r++) {
        // Measure performance of baseline algorithm
        auto time1 = std::chrono::high_resolution_clock::now();
        PostingList list = PostingList::intersectBaseline(lists[i], lists[j]);
        auto time2 = std::chrono::high_resolution_clock::now();

        timeBaseline += std::chrono::duration_cast<std::chrono::microseconds>
                    (time2 - time1).count();
        sizeBaseline = list.size();


        auto time3 = std::chrono::high_resolution_clock::now();
        PostingList list2 = PostingList::intersect(lists[i], lists[j]);
        auto time4 = std::chrono::high_resolution_clock::now();

        timeMy += std::chrono::duration_cast<std::chrono::microseconds>
                    (time4 - time3).count();
        sizeMy = list2.size();
        }
        std::cout << "[Baseline]\t Time: " << timeBaseline/5 << "µs. \t|\t"
                  << "size: " << sizeBaseline << std::endl;
        std::cout << "[My algo ]\t Time: " << timeMy/5 << "µs. \t|\t"
                  << "size: " << sizeMy << std::endl;
        std::cout << "Ratio:    \t    "
                  << round(100.0*timeBaseline/timeMy)/100.0 << std::endl;
        std::cout << std::endl;
    }
  }
  return 0;
}
