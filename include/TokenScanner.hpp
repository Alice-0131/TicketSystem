#ifndef TOKENSCANNER_HPP
#define TOKENSCANNER_HPP

#include <string>

class TokenScanner {
public:
  TokenScanner(std::string& input);
  std::string nextToken();
  bool hasMoreTokens();

private:
  std::string str;
  int index;

  void skipWhitespaces();
};

#endif //TOKENSCANNER_HPP