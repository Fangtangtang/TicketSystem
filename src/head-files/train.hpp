/*
 * TRAIN SYSTEM
 * manage train information
 * deal with cmd
 *         add_train
 *         delete_train
 *         release_train
 *         query_train
 *
 *
 */

#ifndef TICKETSYSTEM_TRAIN_HPP
#define TICKETSYSTEM_TRAIN_HPP


#include "../utility/bpt.hpp"
#include "../utility/pair.hpp"
#include "../utility/tool.hpp"
#include "tokenScanner.hpp"

/*
 * value type stored in train_file
 *       basic information about train,
 *       address of station_name_file,
 *       address of seat_number_file,
 */
class Train {
    int stationNum = 0;
    Time start_sale;
    Time stop_sale;
    long station_addr = 0;
    long seat_addr = 0;
    char type = '\0';
    bool released = false;

public:
    Train() = default;

    Train(const int &stationNum_, const Time &start_sale_, const Time &stop_sale_, const char &type_,
          const long &station, const long &seat);

    bool Release();
};

Train::Train(const int &stationNum_, const Time &start_sale_, const Time &stop_sale_, const char &type_,
             const long &station, const long &seat) :
        stationNum(stationNum_), start_sale(start_sale_), stop_sale(stop_sale_), type(type_),
        station_addr(station), seat_addr(seat) {}

bool Train::Release() {
    if (released)return false;
    released = true;
    return true;
}

#endif //TICKETSYSTEM_TRAIN_HPP
