#include "../include/TokenScanner.hpp"
#include "../include/vector.hpp"

TokenScanner::TokenScanner(std::string& input):str(input), index(0) {}

std::string TokenScanner::nextToken() {
  std::string next;
  skipWhitespaces();
  if (index >= str.length()) {
    return "";
  }
  while (str[index] != ' ' && index < str.length()) {
    next += str[index];
    ++index;
  }
  return next;
}

bool TokenScanner::hasMoreTokens() {
  skipWhitespaces();
  return index < str.length();
}


void TokenScanner::skipWhitespaces() {
  while (index < str.length() && str[index] == ' ') {
    index++;
  }
}

sjtu::vector<std::string> TokenScanner::separate(const std::string &input, std::string flag) {
  sjtu::vector<std::string> keywords;
  std::string keyword;
  for (int i = 0; i < input.length(); ++i) {
    if (input[i] == flag[0]) {
      keywords.push_back(keyword);
      keyword.erase(keyword.begin(), keyword.end());
    } else {
      keyword += input[i];
    }
  }
  keywords.push_back(keyword);
  return keywords;
}