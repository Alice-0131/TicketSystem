#include "../include/Time.hpp"
#include "../include/Ticket.hpp"

Date::Date(int d, int m):day(d), month(m){}

int Date::operator-(const Date &other) const {
  Date tmp(day - other.day, month - other.month);
  if (tmp.month == 1) {
    if (month == 6) {
      return tmp.day + 30;
    } else {
      return tmp.day + 31;
    }
  } else if (tmp.month == 2) {
    return tmp.day + 61;
  } else {
    return tmp.day;
  }
}

Date Date::operator+(const Date &other) const {
  Date tmp(day + other.day, month + other.month);
  if (tmp.month == 6) {
    if (tmp.day > 30) {
      ++tmp.month;
      tmp.day -= 30;
    }
  }
  if (tmp.month == 7) {
    if (tmp.day > 31) {
      ++tmp.month;
      tmp.day -= 31;
    }
  }
  return tmp;
}

bool Date::operator==(const Date &other) const {
  return (month == other.month && day == other.day);
}

bool Date::operator<(const Date &other) const {
  if (month < other.month) {
    return true;
  }
  if (month == other.month) {
    if (day < other.day) {
      return true;
    }
  }
  return false;
}

bool Date::operator>(const Date &other) const {
  if (month > other.month) {
    return true;
  }
  if (month == other.month) {
    if (day > other.day) {
      return true;
    }
  }
  return false;
}

bool Date::operator<=(const Date &other) const {
  return !(*this > other);
}

bool Date::operator>=(const Date &other) const {
  return !(*this < other);
}

std::ostream& operator<<(std::ostream &os, const Date &date) {
  os << 0 << date.month << '-';
  if (date.day < 10) {
    os << 0;
  }
  os << date.day;
  return os;
}

Time::Time(int m, int h, int d): minute(m), hour(h), day(d) {}

void Time::adjust() {
  hour += minute / 60;
  minute %= 60;
  day += hour / 24;
  hour %= 24;
}

Time Time::operator+(const Time &other) const {
  Time tmp(minute + other.minute, hour + other.hour, day + other.day);
  tmp.adjust();
  return tmp;
}

int Time::operator-(const Time &other) const {
  Time tmp(minute - other.minute, hour - other.hour, day - other.day);
  tmp.hour += tmp.day * 24;
  tmp.minute += tmp.hour * 60;
  return tmp.minute;
}


std::ostream& operator<<(std::ostream &os, const Time &time) {
  if (time.hour < 10) {
    os << 0;
  }
  os << time.hour << ':';
  if (time.minute < 10) {
    os << 0;
  }
  os << time.minute;
  return os;
}