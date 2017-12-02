// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@cs.uni-freiburg.de>,
//          Claudius Korzen <korzen@cs.uni-freiburg.de>.

#ifndef POSTINGLIST_H_
#define POSTINGLIST_H_

#include <stddef.h>
#include <vector>
#include <string>

#define INF std::numeric_limits<size_t>::max()

/**
 * A list of postings of form (docId, score).
 */
class PostingList {
 public:
  PostingList(const std::vector<size_t>& ids, const std::vector<size_t>& scores)
  : ids(ids), scores(scores) {}

  PostingList() : ids(), scores() {}

  /**
   * Reads a posting list from the given file.
   */
  void readFromFile(const std::string& fileName);

  /**
   * Intersects the two given posting lists using the basic "zipper" algorithm.
   */
  static PostingList intersectBaseline(const PostingList& list1,
      const PostingList& list2);

  /**
   * Intersects the two given posting lists using an improved algorithm that
   * uses at least three non-trivial ideas presented in the lecture.
   */
  static PostingList intersect(const PostingList& list1,
      const PostingList& list2);

  static PostingList intersect2(const PostingList& list1,
      const PostingList& list2);
  // ==========================================================================

  /**
   * Reserves space for n postings in this list.
   */
  void reserve(size_t n);

  /**
   * Adds the given posting to this list.
   */
  void addPosting(size_t id, size_t score);

  /**
   * Returns the id of the i-th posting.
   */
  size_t getId(size_t i) const;

  /**
   * Returns the score of the i-th posting.
   */
  size_t getScore(size_t i) const;

  /**
   * Returns the number of postings in this list.
   */
  size_t size() const;

  /**
   * Returns index of element id.
   * Return INF when element id is not in array.
   */
  size_t bin_search(size_t first, size_t last, size_t id) const;

  // ==========================================================================

  /**
   * The docIds of the postings in this list.
   */
  std::vector<size_t> ids;

  /**
   * The scores of the postings in this list.
   */
  std::vector<size_t> scores;

  /**
   * The capacity of this list.
   */
  size_t capacity;

  /**
   * The number of postings in this list.
   */
  size_t numPostings;
};

#endif  // POSTINGLIST_H_
