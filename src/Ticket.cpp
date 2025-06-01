#include "../include/Ticket.hpp"
#include "../include/TokenScanner.hpp"

Train::Train(std::string &trainID, int stationNum, int seatNum, std::string &station,
             std::string &prices, std::string &startTime, std::string &travelTimes, std::string &stopoverTimes,
             std::string &salesDate, char type, bool is_released):
StationNum(stationNum), SeatNum(seatNum), type(type), is_released(is_released) {
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

TicketSystem::TicketSystem(): TrainBPT("train_bpt_index", "train_bpt_data"),
StationBPT("station_bpt_index", "station_bpt_data"),
WaitingBPT("waiting_bpt_index", "waiting_bpt_data"),
OrderBPT("order_bpt_index", "order_bpt_data"),
TrainRiver("train_river"), SeatRiver("seat_river"),
StationRiver("station_river"), OrderRiver("order_river"){}

int TicketSystem::add_train(std::string &trainID, int stationNum, int seatNum, std::string &stations,
  std::string &prices, std::string &startTime, std::string &travelTimes, std::string &stopoverTimes,
  std::string &salesDate, std::string &type) {
  long long hash = Hash(trainID);
  if (!TrainBPT.Find(hash).empty()) {
    return -1;
  }
  Train train(trainID, stationNum, seatNum, stations, prices, startTime, travelTimes, stopoverTimes, salesDate, type[0], false);
  int size;
  TrainRiver.get_info(size, 1);
  TrainRiver.open();
  TrainRiver.write(train, size);
  TrainRiver.close();
  TrainBPT.Insert(hash, size);
  TrainRiver.write_info(++size, 1);
  return 0;
}
