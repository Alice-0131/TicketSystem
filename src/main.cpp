#include <iostream>
#include <string>
#include <vector>
#include "../include/TokenScanner.hpp"
#include "../include/User.hpp"

int main() {
  UserSystem user_system;
  while (true) {
    std::string input;
    if (!std::getline(std::cin, input)) {
      //login_stack.end();
      //book_system.end();
      return 0;
    }
    if (input.empty()) {
      continue;
    }
    //processLine(input, login_stack, book_system, diary);
  }
  //login_stack.end();
  //book_system.end();
  return 0;
}

void processLine(std::string& line, UserSystem &user_system) {
  TokenScanner scanner(line);
  std::string token = scanner.nextToken();
  if (token.empty()) {
    return;
  }
  if (token == "exit") {
    //login_stack.end();
    //book_system.end();
    std::cout << "bye";
    exit(0);
  } else if (token == "add_user") {
    std::string cur_username, username, password, name, mailAddr, privilege;
    while (scanner.hasMoreTokens()) {
      token = scanner.nextToken();
      if (token == "-c") {
        cur_username = scanner.nextToken();
      } else if (token == "-u") {
        username = scanner.nextToken();
      } else if (token == "-p") {
        password = scanner.nextToken();
      } else if (token == "-n") {
        name = scanner.nextToken();
      } else if (token == "-m") {
        mailAddr = scanner.nextToken();
      } else if (token == "-g") {
        privilege = scanner.nextToken();
      }
    }
    std::cout << user_system.add_user(cur_username, username, password, name, mailAddr, std::stoi(privilege)) << '\n';
  } else if (token == "login") {
    std::string username, password;
    while (scanner.hasMoreTokens()) {
      token = scanner.nextToken();
      if (token == "-u") {
        username = scanner.nextToken();
      } else if (token == "-p") {
        password = scanner.nextToken();
      }
    }
    std::cout << user_system.login(username, password) << '\n';
  } else if (token == "logout") {
    std::string username;
    while (scanner.hasMoreTokens()) {
      token = scanner.nextToken();
      if (token == "-u") {
        username = scanner.nextToken();
      }
    }
    std::cout << user_system.logout(username) << '\n';
  } else if (token == "query_profile") {
    std::string cur_username, username;
    while (scanner.hasMoreTokens()) {
      token = scanner.nextToken();
      if (token == "-c") {
        cur_username = scanner.nextToken();
      } else if (token == "-u") {
        username = scanner.nextToken();
      }
    }
    std::pair<User, bool> pair_ = user_system.query_profile(cur_username, username);
    if (!pair_.second) {
      std::cout << -1 << '\n';
    } else {
      std::cout << pair_.first << '\n';
    }
  } else if (token == "modify_profile") {
    std::string cur_username, username, password, name, mailAddr, privilege;
    while (scanner.hasMoreTokens()) {
      token = scanner.nextToken();
      if (token == "-c") {
        cur_username = scanner.nextToken();
      } else if (token == "-u") {
        username = scanner.nextToken();
      } else if (token == "-p") {
        password = scanner.nextToken();
      } else if (token == "-n") {
        name = scanner.nextToken();
      } else if (token == "-m") {
        mailAddr = scanner.nextToken();
      } else if (token == "-g") {
        privilege = scanner.nextToken();
      }
    }
    if (privilege.empty()) {
      privilege = "-1";
    }
    std::pair<User, bool> pair_ = user_system.modify_profile(cur_username, username, password, name, mailAddr, std::stoi(privilege));
    if (!pair_.second) {
      std::cout << -1 << '\n';
    } else {
      std::cout << pair_.first << '\n';
    }
  } else if (token == "add_train") {

  }
}