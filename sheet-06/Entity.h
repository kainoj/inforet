// Copyright 2017, University of Freiburg
// Author: Przemyslaw Joniak,

#ifndef ENTITY_H_
#define ENTITY_H_

#include <string>
#include <vector>

using std::string;
using std::tuple;

class Entity {
 public:
  explicit Entity(string name, size_t score, string description)
    : name(name),
      score(score),
      description(description) {}

  string getName() const {
    return name;
  }

  size_t getScore() const {
    return score;
  }

  string getDesc() const {
    return description;
  }

  // Get description without url
  string getTruncDesc() const {
    size_t pos = description.find("http");
    if (pos != string::npos)
      return description.substr(0, pos-1);
    return description;
  }

  friend std::ostream& operator<< (std::ostream& stream, const Entity& e) {
    stream << "name: " << e.getName() << ", ";
    stream << "score: " << e.getScore() << ", ";
    stream << "description: " << e.getDesc();
    return stream;
  }

  bool operator==(const Entity e) const {
    return name == e.getName()
       && score == e.getScore()
       && description == e.getDesc();
  }

 private:
  string name;
  size_t score;
  string description;
};

#endif  // ENTITY_H_
