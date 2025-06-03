#ifndef TIME_HPP
#define TIME_HPP

#include <ostream>

struct Date{
  int month;
  int day;

  Date(int d = 0, int m = 0);
  int operator-(const Date &other) const;
  Date operator+(const Date &other) const;
  bool operator==(const Date &other) const;
  bool operator<(const Date &other) const;
  bool operator>(const Date &other) const;
  bool operator<=(const Date &other) const;
  bool operator >=(const Date &other) const;
  friend std::ostream& operator<<(std::ostream &os, const Date &date);
};

struct Time{
  int day;
  int hour;
  int minute;

  void adjust();
  Time(int m = 0, int h = 0, int d = 0);
  Time operator+(const Time &other) const;
  int operator-(const Time &other) const;
  bool operator<(const Time &other) const; // 不管day
  friend std::ostream& operator<<(std::ostream &os, const Time &time);
};

#endif //TIME_HPP