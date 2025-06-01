#ifndef TOKENSCANNER_HPP
#define TOKENSCANNER_HPP

#include <string>

#include "map.hpp"
#include "vector.hpp"

class TokenScanner {
public:
  TokenScanner(std::string& input);
  std::string nextToken();
  bool hasMoreTokens();
  static sjtu::vector<std::string> separate(const std::string &input, std::string flag);
private:
  std::string str;
  int index;

  void skipWhitespaces();
};

#endif //TOKENSCANNER_HPP