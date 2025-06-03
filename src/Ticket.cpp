#include "../include/Ticket.hpp"
#include "../include/priority_queue.hpp"
#include "../include/TokenScanner.hpp"

Train::Train(std::string &trainID, int stationNum, int seatNum, std::string &station,
             std::string &prices, std::string &startTime, std::string &travelTimes, std::string &stopoverTimes,
             std::string &salesDate, char type, bool is_released, int seat_no):
StationNum(stationNum), SeatNum(seatNum), type(type), is_released(is_released), seat_no(seat_no){
  strcpy(TrainID, trainID.c_str());
  sjtu::vector<std::string> station_name = TokenScanner::separate(station, "|");
  sjtu::vector<std::string> price = TokenScanner::separate(prices, "|");
  sjtu::vector<std::string> start_time = TokenScanner::separate(startTime, ":");
  sjtu::vector<std::string> travel_time = TokenScanner::separate(travelTimes, ":");
  sjtu::vector<std::string> stopover_time = TokenScanner::separate(stopoverTimes, "|");
  sjtu::vector<std::string> sale_date = TokenScanner::separate(salesDate, "|");
  sjtu::vector<std::string> sale_start = TokenScanner::separate(sale_date[0], "-");
  sjtu::vector<std::string> sale_end = TokenScanner::separate(sale_date[1], "-");
  strcpy(stations[0].StationName, station_name[0].c_str());
  stations[0].LeavingTime = {std::stoi(start_time[1]), std::stoi(start_time[0])};
  int i = 1;
  for (; i < stationNum - 1; ++i) {
    strcpy(stations[i].StationName, station_name[i].c_str());
    stations[i].ArrivingTime = stations[i - 1].LeavingTime + std::stoi(travel_time[i - 1]);
    stations[i].LeavingTime = stations[i].ArrivingTime + std::stoi(stopover_time[i - 1]);
    stations[i].Price = stations[i - 1].Price + std::stoi(price[i - 1]);
  }
  strcpy(stations[i].StationName, station_name[i].c_str());
  stations[i].ArrivingTime = stations[i - 1].LeavingTime + std::stoi(travel_time[i - 1]);
  stations[i].Price = stations[i - 1].Price + std::stoi(price[i - 1]);;
  SaleStart = {std::stoi(sale_start[1]), std::stoi(sale_start[0])};
  SaleEnd = {std::stoi(sale_end[1]), std::stoi(sale_end[0])};
}

Ticket::Ticket(Date &leaving_date, Time &leaving_time, Time &arriving_time, int price,int num):
LeavingDate(leaving_date), LeavingTime(leaving_time),ArrivingTime(arriving_time),price(price), num(num) {
  time = arriving_time - leaving_time;
}

std::ostream& operator<<(std::ostream &os, const Ticket &ticket) {
  os << ticket.TrainID << ' ' << ticket.From << ' ';
  os << ticket.LeavingDate + ticket.LeavingTime.day << ' ';
  os << ticket.LeavingTime << " -> " << ticket.To;
  os << ticket.LeavingDate + ticket.ArrivingTime.day << ' ';
  os << ticket.ArrivingTime << ' ' << ticket.price << ' ' << ticket.num;
  return os;
}

TransferTicket::TransferTicket(Ticket ticket1, Ticket ticket2): ticket1(ticket1), ticket2(ticket2) {
  time = ticket2.ArrivingTime - ticket1.LeavingTime;
}

int TransferTicket::cmp_t(TransferTicket &a, TransferTicket &b) {
  if (a.time > b.time) {
    return 1;
  } else if (a.time < b.time) {
    return -1;
  }
  if (a.ticket1.price + a.ticket2.price > b.ticket1.price + b.ticket2.price) {
    return 1;
  } else if (a.ticket1.price + a.ticket2.price < b.ticket1.price + b.ticket2.price) {
    return -1;
  }
  if (strcmp(a.ticket1.TrainID, b.ticket1.TrainID) != 0) {
    return strcmp(a.ticket1.TrainID, b.ticket1.TrainID);
  }
  return strcmp(a.ticket2.TrainID, b.ticket2.TrainID);
}

int TransferTicket::cmp_c(TransferTicket &a, TransferTicket &b) {
  if (a.ticket1.price + a.ticket2.price > b.ticket1.price + b.ticket2.price) {
    return 1;
  } else if (a.ticket1.price + a.ticket2.price < b.ticket1.price + b.ticket2.price) {
    return -1;
  }
  if (a.time > b.time) {
    return 1;
  } else if (a.time < b.time) {
    return -1;
  }
  if (strcmp(a.ticket1.TrainID, b.ticket1.TrainID) != 0) {
    return strcmp(a.ticket1.TrainID, b.ticket1.TrainID);
  }
  return strcmp(a.ticket2.TrainID, b.ticket2.TrainID);
}

std::ostream& operator<<(std::ostream &os, const TransferTicket &transfer_ticket) {
  os << transfer_ticket.ticket1 << '\n' << transfer_ticket.ticket2;
  return os;
}

bool StationInfo::operator<(const StationInfo &other) const{
  return train_no < other.train_no;
}
bool StationInfo::operator==(const StationInfo &other) const {
  return train_no == other.train_no;
}

TicketSystem::TicketSystem(): TrainBPT("train_bpt_index", "train_bpt_data"),
StationBPT("station_bpt_index", "station_bpt_data"),
WaitingBPT("waiting_bpt_index", "waiting_bpt_data"),
OrderBPT("order_bpt_index", "order_bpt_data"),
TrainRiver("train_river"), SeatRiver("seat_river"),OrderRiver("order_river"){}

int TicketSystem::add_train(std::string &trainID, int stationNum, int seatNum, std::string &stations,
  std::string &prices, std::string &startTime, std::string &travelTimes, std::string &stopoverTimes,
  std::string &salesDate, std::string &type) {
  long long hash = Hash(trainID);
  if (!TrainBPT.Find(hash).empty()) {
    return -1;
  }
  int size;
  SeatRiver.open();
  SeatRiver.get_info(size, 1);
  Train train(trainID, stationNum, seatNum, stations, prices, startTime, travelTimes,
    stopoverTimes, salesDate, type[0], false, size);
  Seat seat;
  for (int i = 0; i < train.SaleEnd - train.SaleStart; ++i) {
    SeatRiver.write(seat, size++);
  }
  SeatRiver.write_info(size, 1);
  SeatRiver.close();
  TrainRiver.open();
  TrainRiver.get_info(size, 1);
  TrainRiver.write(train, size);
  TrainBPT.Insert(hash, size);
  TrainRiver.write_info(size + 1, 1);
  TrainRiver.close();
  return 0;
}

int TicketSystem::delete_train(std::string &trainID) {
  long long hash = Hash(trainID);
  sjtu::vector<int> no = TrainBPT.Find(hash);
  if (no.empty()) {
    return -1;
  }
  Train train;
  TrainRiver.open();
  TrainRiver.read(train, no[0]);
  TrainRiver.close();
  if (train.is_released) {
    return -1;
  }
  TrainBPT.Delete(hash, no[0]);
  return 0;
}

int TicketSystem::release_train(std::string &trainID) {
  long long hash = Hash(trainID);
  sjtu::vector<int> no = TrainBPT.Find(hash);
  if (no.empty()) {
    return -1;
  }
  Train train;
  TrainRiver.open();
  TrainRiver.read(train, no[0]);
  if (train.is_released) {
    TrainRiver.close();
    return -1;
  }
  train.is_released = true;
  TrainRiver.write(train, no[0]);
  TrainRiver.close();
  for (int i = 0; i < train.StationNum; ++i) {
    std::string str(train.stations[i].StationName);
    StationBPT.Insert(Hash(str), {no[0], i});
  }
  return 0;
}

void TicketSystem::query_train(std::string &trainID, std::string &date) {
  Train train;
  long long hash = Hash(trainID);
  sjtu::vector<int> no = TrainBPT.Find(hash);
  if (no.empty()) {
    std::cout << -1;
    return;
  }
  TrainRiver.open();
  TrainRiver.read(train, no[0]);
  TrainRiver.close();
  sjtu::vector<std::string> v = TokenScanner::separate(date, "-");
  Date d(std::stoi(v[1]), std::stoi(v[0]));
  if (d < train.SaleStart || d > train.SaleEnd) {
    std::cout << "-1";
    return ;
  }
  Seat seat;
  SeatRiver.open();
  SeatRiver.read(seat, train.seat_no + (d - train.SaleStart));
  SeatRiver.close();
  std::cout << train.TrainID << ' ' << train.type << '\n';
  int seat_num = train.SeatNum;
  std::cout << train.stations[0].StationName;
  std::cout << " xx-xx xx:xx -> ";
  std::cout << d + train.stations[0].LeavingTime.day << ' ' << train.stations[0].LeavingTime << ' ';
  std::cout << train.stations[0].Price << ' ';
  seat_num += seat.seat[0];
  std::cout << seat_num << '\n';
  int i = 1;
  for (; i < train.StationNum - 1; ++i) {
    std::cout << train.stations[i].StationName << ' ';
    std::cout << d + train.stations[i].ArrivingTime.day << ' ' << train.stations[i].ArrivingTime << " -> ";
    std::cout << d + train.stations[i].LeavingTime.day << ' ' << train.stations[i].LeavingTime << ' ';
    std::cout << train.stations[i].Price << ' ';
    seat_num += seat.seat[i];
    std::cout << seat_num << '\n';
  }
  std::cout << train.stations[i].StationName << ' ';
  std::cout << d + train.stations[i].ArrivingTime.day << ' ' << train.stations[i].ArrivingTime ;
  std::cout << " -> xx-xx xx:xx ";
  std::cout << train.stations[i].Price << ' ';
  seat_num += seat.seat[i];
  std::cout << seat_num;
}

void TicketSystem::query_ticket(std::string &from, std::string &to, std::string &date, std::string &p) {
  long long hash_from = Hash(from);
  long long hash_to = Hash(to);
  sjtu::vector<StationInfo> from_no = StationBPT.Find(hash_from);
  sjtu::vector<StationInfo> to_no = StationBPT.Find(hash_to);
  sjtu::vector<StationInfo> fno;
  sjtu::vector<StationInfo> tno;
  int i = 0, j = 0;
  while (i < from_no.size() && j < to_no.size()) {
    while (i < from_no.size() && from_no[i] < to_no[j]) ++i;
    if (i < from_no.size() && from_no[i] == to_no[j] && from_no[i].index < to_no[j].index) {
      fno.push_back(from_no[i]);
      tno.push_back(to_no[i]);
      ++i, ++j;
    }
    while (j < to_no.size() && to_no[j] < from_no[i]) ++j;
    if (j < to_no.size() && from_no[i] == to_no[j] && from_no[i].index < to_no[j].index) {
      fno.push_back(from_no[i]);
      tno.push_back(to_no[i]);
      ++i, ++j;
    }
  }
  sjtu::vector<std::string> v = TokenScanner::separate(date, "-");
  const Date d(stoi(v[1]), stoi(v[0]));
  int sum = 0;
  if (p == "time") {
    sjtu::priority_queue<Ticket, cmp_t> pq;
    TrainRiver.open();
    Train train;
    for (int i = 0; i < fno.size(); ++i) {
      TrainRiver.read(train, fno[i].train_no);
      int f = from_no[i].index;
      int t = to_no[i].index;
      if (d < train.SaleStart + train.stations[f].LeavingTime.day ||
        d > train.SaleEnd + train.stations[f].LeavingTime.day) {
        continue;
      }
      ++sum;
      Seat seat;
      int seat_num = train.SeatNum, min = train.SeatNum;
      SeatRiver.open();
      SeatRiver.read(seat, train.seat_no + (d - train.SaleStart- train.stations[f].LeavingTime.day));
      int j = 0;
      for (; j < f; ++j) {
        seat_num += seat.seat[j];
      }
      for (; j < t; ++j) {
        seat_num += seat.seat[j];
        min = std::max(min, seat_num);
      }
      Date da = d - train.stations[f].LeavingTime.day;
      Ticket ticket(da, train.stations[f].LeavingTime,train.stations[t].ArrivingTime,
        train.stations[t].Price - train.stations[f].Price, min);
      strcpy(ticket.From, train.stations[f].StationName);
      strcpy(ticket.To, train.stations[t].StationName);
      strcpy(ticket.TrainID, train.TrainID);
      pq.push(ticket);
    }
    TrainRiver.close();
    std::cout << sum << '\n';
    while (!pq.empty()) {
      Ticket ticket = pq.top();
      pq.pop();
      std::cout << ticket << '\n';
    }
  } else if (p == "cost") {
    sjtu::priority_queue<Ticket, cmp_c> pq;
    TrainRiver.open();
    Train train;
    for (int i = 0; i < fno.size(); ++i) {
      TrainRiver.read(train, fno[i].train_no);
      int f = from_no[i].index;
      int t = to_no[i].index;
      if (d < train.SaleStart + train.stations[f].LeavingTime.day ||
        d > train.SaleEnd + train.stations[f].LeavingTime.day) {
        continue;
        }
      Seat seat;
      int seat_num = train.SeatNum, min = train.SeatNum;
      SeatRiver.open();
      SeatRiver.read(seat, train.seat_no + (d - train.SaleStart- train.stations[f].LeavingTime.day));
      int j = 0;
      for (; j < f; ++j) {
        seat_num += seat.seat[j];
      }
      for (; j < t; ++j) {
        seat_num += seat.seat[j];
        min = std::max(min, seat_num);
      }
      Date da = d - train.stations[f].LeavingTime.day;
      Ticket ticket(da, train.stations[f].LeavingTime,
        train.stations[t].ArrivingTime, train.stations[t].Price - train.stations[f].Price, min);
      strcpy(ticket.From, train.stations[f].StationName);
      strcpy(ticket.To, train.stations[t].StationName);
      strcpy(ticket.TrainID, train.TrainID);
      pq.push(ticket);
    }
    TrainRiver.close();
    std::cout << sum << '\n';
    while (!pq.empty()) {
      Ticket ticket = pq.top();
      pq.pop();
      std::cout << ticket << '\n';
    }
  }
}

void TicketSystem::query_transfer(std::string &from, std::string &to, std::string &date, std::string &p) {
  long long hash_from = Hash(from);
  long long hash_to = Hash(to);
  sjtu::vector<StationInfo> from_no = StationBPT.Find(hash_from);
  sjtu::vector<StationInfo> to_no = StationBPT.Find(hash_to);
  sjtu::vector<std::string> v = TokenScanner::separate(date, "-");
  const Date d(stoi(v[1]), stoi(v[0]));
  Train train1, train2;
  TransferTicket min_transfer_ticket;
  bool flag = false;
  TrainRiver.open();
  SeatRiver.open();
  int f, t;
  for (int i = 0; i < from_no.size(); ++i) {
    TrainRiver.read(train1, from_no[i].train_no);
    f = from_no[i].index;
    if (d < train1.SaleStart + train1.stations[f].LeavingTime.day ||
      d > train1.SaleEnd + train1.stations[f].LeavingTime.day) {
      continue;
    }
    Date train1_leaving_date = d - train1.stations[f].LeavingTime.day;
    for (int k = 0; k < to_no.size(); ++k) {
      TrainRiver.read(train2, to_no[k].train_no);
      t = to_no[k].index;
      if (d < train2.SaleStart || d > train2.SaleEnd + 3 || from_no[i].train_no == to_no[k].train_no) {
        continue;
      }
      for (int j = f + 1; j < train1.StationNum; ++j) {
        for (int l = 0; l < t; ++l) {
          if (strcmp(train1.stations[j].StationName, train2.stations[l].StationName) != 0) {
            continue;
          }
          Time arrive_transfer_time = train1.stations[j].ArrivingTime;
          Time leave_transfer_time = train2.stations[l].LeavingTime;
          if (train2.SaleEnd + leave_transfer_time.day < train1_leaving_date + arrive_transfer_time.day) {
            continue;
          }
          if (train2.SaleEnd + leave_transfer_time.day == train1_leaving_date + arrive_transfer_time.day &&
            leave_transfer_time < arrive_transfer_time) {
            continue;
          }
          Date train2_leaving_date;
          if (train2.SaleStart + leave_transfer_time.day > train1_leaving_date + arrive_transfer_time.day ||
            train2.SaleStart + leave_transfer_time.day > train1_leaving_date + arrive_transfer_time.day &&
            !(leave_transfer_time < arrive_transfer_time)) { // 最早班车即可换乘
            train2_leaving_date = train2.SaleStart;
          } else {
            if (leave_transfer_time < arrive_transfer_time) { // 要延后一天换乘
              train2_leaving_date = train1_leaving_date + (arrive_transfer_time.day - leave_transfer_time.day + 1);
            } else {
              train2_leaving_date = train1_leaving_date + (arrive_transfer_time.day - leave_transfer_time.day);
            }
          }
          int num1 = train1.SeatNum, num2 = train2.SeatNum;
          int min1 = num1, min2 = num2;
          Seat seat1, seat2;
          SeatRiver.read(seat1, train1.seat_no + (train1_leaving_date - train1.SaleStart));
          SeatRiver.read(seat2, train2.seat_no + (train2_leaving_date - train2.SaleStart));
          int x = 0;
          for (; x < f; ++x) {
            num1 += seat1.seat[x];
          }
          for (; x < j; ++x) {
            num1 += seat1.seat[x];
            min1 = std::min(min1, num1);
          }
          x = 0;
          for (; x < l; ++x) {
            num2 += seat2.seat[x];
          }
          for (; x < t; ++x) {
            num2 += seat2.seat[x];
            min2 = std::min(min2, num2);
          }
          Ticket ticket1(train1_leaving_date, train1.stations[f].LeavingTime,
            arrive_transfer_time, train1.stations[j].Price - train1.stations[f].Price, min1);
          Ticket ticket2(train2_leaving_date, leave_transfer_time,
            train2.stations[t].ArrivingTime, train2.stations[t].Price - train2.stations[l].Price, min2);
          TransferTicket transfer_ticket(ticket1, ticket2);
          if (!flag) {
            min_transfer_ticket = transfer_ticket;
            flag = true;
          } else {
            if (p == "time") {
              if (TransferTicket::cmp_t(transfer_ticket, min_transfer_ticket) < 0) {
                min_transfer_ticket = transfer_ticket;
              }
            } else {
              if (TransferTicket::cmp_c(transfer_ticket, min_transfer_ticket) < 0) {
                min_transfer_ticket = transfer_ticket;
              }
            }
          }
        }
      }
    }
  }
  if (!flag) {
    std::cout << "0\n";
  } else {
    std::cout << min_transfer_ticket << '\n';
  }
  TrainRiver.close();
  SeatRiver.close();
}
