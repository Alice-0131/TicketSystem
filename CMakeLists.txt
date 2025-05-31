cmake_minimum_required(VERSION 3.22)
project(TicketSystem)

set(CMAKE_CXX_STANDARD 20)

add_executable(TicketSystem src/main.cpp
        include/vector.hpp
        include/BPT.hpp
        include/map.hpp
        include/priority_queue.hpp
        include/exceptions.hpp
        include/utility.hpp
        include/MemoryRiver.hpp
        include/TokenScanner.hpp
        src/TokenScanner.cpp
        src/User.cpp
        include/User.hpp
        src/Train.cpp
        include/Train.hpp
        src/Ticket.cpp
        include/Ticket.hpp
)
