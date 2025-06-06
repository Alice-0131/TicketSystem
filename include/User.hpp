#ifndef USER_HPP
#define USER_HPP
#include "map.hpp"
#include "vector.hpp"
#include "BPT.hpp"
#include <string>

struct User {
  char Username[21] = {0};
  char Password[31] = {0};
  char Name[21] = {0};
  char MailAddr[31] = {0};
  int Privilege = 0;

  User() = default;
  User(const std::string &Username, const std::string &Password, const std::string &Name,
    const std::string &MailAddr, int Privilege);
  friend std::ostream& operator<<(std::ostream &os, const User &user);
};

class UserSystem {
  friend class TicketSystem;
private:
  sjtu::map<long long, int> stack; // 哈希的username，对应的privilege，若未登入则为-1
  BPT<> UserBPT;
  MemoryRiver<User> UserRiver;

public:
  UserSystem();
  int add_user(std::string &cur_username, std::string &username, std::string &password,
    std::string &name, std::string &mailAddr, int privilege);
  int login(std::string &username, std::string &password);
  int logout(std::string &username);
  void query_profile(std::string &cur_username, std::string &username);
  void modify_profile(std::string &cur_username, std::string &username, std::string &password,
    std::string &name, std::string &mailAddr, int privilege);
  void clean();
  void end() {
    UserBPT.end();
  }
};

#endif //USER_HPP