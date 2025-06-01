#include "../include/Time.hpp"

#include <bits/ranges_algo.h>


#include "../include/Ticket.hpp"
Date::Date(int d, int m):day(d), month(m){}

Time::Time(int m, int h, int d): minute(m), hour(h), day(d) {}

Time Time::operator+(const Time &other) {
  Time tmp(minute + other.minute, hour + other.hour, day + other.day);
  tmp.adjust();
  return tmp;
}

void Time::adjust() {
  hour += minute / 60;
  minute %= 60;
  day += hour / 24;
  hour %= 24;
}
