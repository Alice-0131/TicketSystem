#include <cstring>
#include "../include/User.hpp"
#include "../include/BPT.hpp"

User::User(const std::string &Username, const std::string &Password, const std::string &Name,
  const std::string &MailAddr, int Privilege): Privilege(Privilege) {
  strcpy(this->Username, Username.c_str());
  strcpy(this->Password, Password.c_str());
  strcpy(this->Name, Name.c_str());
  strcpy(this->MailAddr, MailAddr.c_str());
}

UserSystem::UserSystem():UserBPT("user_bpt_index", "user_bpt_data"), UserRiver("user_river"){}

int UserSystem::add_user(std::string &cur_username, std::string &username, std::string &password, std::string &name,
  std::string &mailAddr, int privilege) {
  int size;
  long long hash = Hash(username);
  UserRiver.get_info(size, 1);
  if (size == 0) { // 建立初始账户
    User user(username, password, name, mailAddr, 10);
    UserRiver.open();
    UserRiver.write(user, 0);
    UserRiver.close();
    UserBPT.Insert(hash, 0);
    UserRiver.write_info(1, 1);
    return 0;
  }
  long long cur_hash = Hash(cur_username);
  if (!stack.count(cur_hash) || stack[cur_hash] <= privilege) { // 权限不够或未登录
    return -1;
  }
  sjtu::vector<int> no = UserBPT.Find(hash);
  if (!no.empty()) { // 账户已存在
    return -1;
  }
  User user(username, password, name, mailAddr, privilege);
  UserRiver.open();
  UserRiver.write(user, size);
  UserRiver.close();
  UserBPT.Insert(hash, size++);
  UserRiver.write_info(size, 1);
  return 0;
}

int UserSystem::login(std::string &username, std::string &password) {
  long long hash = Hash(username);
  sjtu::vector<int> no = UserBPT.Find(hash);
  if (no.empty()) { // 账户不存在
    return -1;
  }
  User user;
  UserRiver.open();
  UserRiver.read(user, no[0]);
  UserRiver.close();
  if (strcmp(user.Password, password.c_str()) != 0){ // 密码错误
    return -1;
  }
  if (stack.count(hash)) {
    if (stack[hash] >= 0) { // 已登录
      return -1;
    }
  }
  stack[hash] = user.Privilege;
  return 0;
}

int UserSystem::logout(std::string &username) {
  long long hash = Hash(username);
  if (!stack.count(hash) || stack[hash] == -1) { // 未登录
    return -1;
  }
  stack[hash] = -1;
  return 0;
}

std::pair<User, bool> UserSystem::query_profile(std::string &cur_username, std::string &username) {
  User user;
  long long cur_hash = Hash(cur_username);
  long long hash = Hash(username);
  if (!stack.count(cur_hash) || stack[cur_hash] == -1) { // 未登录
    return {user, false};
  }
  sjtu::vector<int> no = UserBPT.Find(hash);
  if (no.empty()) { // 没有该账户
    return {user, false};
  }
  UserRiver.open();
  UserRiver.read(user, no[0]);
  UserRiver.close();
  if (stack[cur_hash] <= user.Privilege && cur_hash != hash) { // 权限不够
    return {user, false};
  }
  return {user, true};
}

std::pair<User, bool> UserSystem::modify_profile(std::string &cur_username, std::string &username, std::string &password,
  std::string &name, std::string &mailAddr, int privilege) {
  User user;
  long long cur_hash = Hash(cur_username);
  long long hash = Hash(username);
  if (!stack.count(cur_hash) || stack[cur_hash] == -1) { // 未登录
    return {user, false};
  }
  sjtu::vector<int> no = UserBPT.Find(hash);
  if (no.empty()) { // 没有该账户
    return {user, false};
  }
  UserRiver.open();
  UserRiver.read(user, no[0]);
  if (stack[cur_hash] <= user.Privilege && cur_hash != hash || privilege >= stack[cur_hash]) { // 权限不够
    return {user, false};
  }
  if (!password.empty()) {
    strcpy(user.Password, password.c_str());
  }
  if (!name.empty()) {
    strcpy(user.Name, name.c_str());
  }
  if (!mailAddr.empty()) {
    strcpy(user.MailAddr, mailAddr.c_str());
  }
  if (privilege != -1) {
    user.Privilege = privilege;
  }
  UserRiver.write(user, no[0]);
  UserRiver.close();
  return {user, true};
}
