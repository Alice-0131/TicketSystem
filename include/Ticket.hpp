#ifndef TICKET_HPP
#define TICKET_HPP

#include <string>
#include "Time.hpp"
#include "vector.hpp"
#include "BPT.hpp"
#include "User.hpp"

struct Station {
  char StationName[32]{};
  Time ArrivingTime;
  Time LeavingTime;
  int Price{}; // 累计
  //int DeltSeatNum[100]{}; // 差分数组

  Station() = default;
};

struct Train {
  char TrainID[22]{};
  int StationNum{};
  int SeatNum{};
  Station stations[102];
  Date SaleStart;
  Date SaleEnd;
  char type{};
  bool is_released{};
  int seat_no{};

  Train() = default;
  Train(std::string &trainID, int stationNum, int seatNum, std::string &stations,
  std::string &prices, std::string &startTime, std::string &travelTimes, std::string &stopoverTimes,
  std::string &salesDate, char type, bool is_released, int seat_no);
};

struct Seat {
  int seat[102]{};
  int max;
};

struct Ticket {
  char TrainID[22]{};
  char From[32]{};
  int f;
  Date LeavingDate; // 存的是列车始发站的日期
  Time LeavingTime;
  char To[32]{};
  int t;
  Time ArrivingTime;
  int price{};
  int num{};
  int time{};

  Ticket() = default;
  Ticket(Date &leaving_date, Time &leaving_time, Time &arriving_time, int price, int num, int f, int t);
  friend std::ostream& operator<<(std::ostream &os, const Ticket &ticket);
};

struct cmp_t {
  bool operator()(const Ticket &a, const Ticket &b) const {
    if (a.time != b.time) {
      return a.time > b.time;
    }
    return strcmp(a.TrainID, b.TrainID);
  }
};

struct cmp_c {
  bool operator()(const Ticket &a, const Ticket &b) {
    if (a.price != b.price) {
      return a.price > b.price;
    }
    if (strcmp(a.TrainID, b.TrainID) > 0) {
      return true;
    } else {
      return false;
    }
  }
};

struct TransferTicket {
  Ticket ticket1;
  Ticket ticket2;
  int time{};

  TransferTicket() = default;
  TransferTicket(Ticket ticket1, Ticket ticket2);

  friend std::ostream& operator<<(std::ostream &os, const TransferTicket &transfer_ticket);

  static int cmp_t(TransferTicket &a, TransferTicket &b);
  static int cmp_c(TransferTicket &a, TransferTicket &b);
};

struct Order {
  Ticket ticket;
  int status; // 0:pending, 1:success, -1:refunded

  Order() = default;
  Order(Ticket ticket, int status):ticket(ticket), status(status){}

  friend std::ostream& operator<<(std::ostream &os, const Order &order);
};

struct StationInfo{
  int train_no; // train所在文件的下标
  int index; // station是train中的第几站

  bool operator<(const StationInfo &other) const;
  bool operator==(const StationInfo &) const;
};

struct WaitingInfo {
  long long trainID;
  int date;

  WaitingInfo() = default;
  WaitingInfo(long long trainID, int date):trainID(trainID), date(date){}

  bool operator==(const WaitingInfo &) const;
  bool operator<(const WaitingInfo &other) const;
  bool operator>(const WaitingInfo &other) const;
};

class TicketSystem {
private:
  BPT<> TrainBPT;
  MemoryRiver<Train> TrainRiver;
  MemoryRiver<Seat> SeatRiver;
  BPT<long long , StationInfo> StationBPT;
  BPT<WaitingInfo> WaitingBPT;
  BPT<> OrderBPT;
  MemoryRiver<Order> OrderRiver;

public:
  TicketSystem();
  int add_train(std::string &trainID, int stationNum, int seatNum, std::string &stations,
    std::string &prices, std::string &startTime, std::string &travelTimes,
    std::string &stopoverTimes, std::string &salesDate, std::string &type);
  int delete_train(std::string &trainID);
  int release_train(std::string &trainID);
  void query_train(std::string &trainID, std::string &Date);
  void query_ticket(std::string &from, std::string &to, std::string &date, std::string &p);
  void query_transfer(std::string &from, std::string &to, std::string &date, std::string &p);
  void buy_ticket(std::string &username, std::string &trainID, std::string &date,
    std::string &from, std::string &to, int n, std::string &q, UserSystem &user_system);
  void query_order(std::string &username, UserSystem &user_system);
  int refund_ticket(std::string &username, int n, UserSystem &user_system);
  void clean();
  void end() {
    TrainBPT.end();
    StationBPT.end();
    WaitingBPT.end();
    OrderBPT.end();
  }
};

#endif //TICKET_HPP