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
  int DeltSeatNum[100]{}; // 差分数组

  Station() = default;
};

struct Train {
  char TrainID[22];
  int StationNum;
  int SeatNum;
  Station stations[102];
  Date SaleStart;
  Date SaleEnd;
  char type;
  bool is_released;

  Train(std::string &trainID, int stationNum, int seatNum, std::string &stations,
  std::string &prices, std::string &startTime, std::string &travelTimes, std::string &stopoverTimes,
  std::string &salesDate, char type, bool is_released);
};

struct StationInfo {
  int index; // 该station是第几站
  int no; // 该station对应的列车所在文件的下标
  char trainID[22];
  Date start_date;
  Date end_date;
  Time arriving_time;
  Time Leaving_time;
};

struct Seat {
  int seat[102];
};

struct Ticket {
  char TrainID[22];
  char From[32];
  Date LeavingDate;
  Time LeavingTime;
  char To[32];
  Date ArrivingDate;
  Time ArrivingTime;
  int price;
  int num;
};

struct TransferTicket {
  Ticket ticket1;
  Ticket ticket2;
};

struct Order {
  Ticket ticket;
  int status;
};

class TicketSystem {
private:
  BPT<long long> TrainBPT;
  MemoryRiver<Train> TrainRiver;
  MemoryRiver<Seat> SeatRiver;
  BPT<long long> StationBPT;
  MemoryRiver<StationInfo> StationRiver;
  BPT<std::pair<long long, Date>> WaitingBPT;
  BPT<long long> OrderBPT;
  MemoryRiver<Order> OrderRiver;

public:
  TicketSystem();
  int add_train(std::string &trainID, int stationNum, int seatNum, std::string &stations,
    std::string &prices, std::string &startTime, std::string &travelTimes,
    std::string &stopoverTimes, std::string &salesDate, std::string &type);
  int delete_train(std::string &trainID);
  int release_train(std::string &trainID);
  std::pair<Train, bool> query_train(std::string &trainID, std::string &Date);
  std::pair<Ticket, bool> query_ticket(std::string &from, std::string &to, std::string &date, std::string &p);
  std::pair<TransferTicket, bool> query_transfer(std::string &from, std::string &to, std::string &date, std::string &p);
  std::string buy_ticket(std::string &username, std::string &trainID,
    std::string &date, std::string &from, std::string &to, int n, UserSystem &user_system);
  sjtu::vector<Order> &query_order(std::string &username, UserSystem &user_system);
  int refund_ticket(std::string &username, int n, UserSystem &user_system);
  void clean();
};

#endif //TICKET_HPP
