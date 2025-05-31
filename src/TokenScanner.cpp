#include "../include/TokenScanner.hpp"

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