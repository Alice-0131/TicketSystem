#ifndef TIME_HPP
#define TIME_HPP

struct Date{
  int month;
  int day;

  Date(int d = 0, int m = 0);
};

struct Time{
  int day;
  int hour;
  int minute;

  Time(int m = 0, int h = 0, int d = 0);
  Time operator+(const Time &other);
  void adjust();
};

#endif //TIME_HPP
