/*
 * TRAIN SYSTEM
 * manage train information
 * deal with cmd
 *         add_train
 *         delete_train
 *         release_train
 *         query_train
 *         buy_ticket
 */

#ifndef TICKETSYSTEM_TRAIN_HPP
#define TICKETSYSTEM_TRAIN_HPP


#include "../utility/bpt.hpp"
#include "../utility/pair.hpp"
#include "../utility/tool.hpp"
#include "../utility/file_manager.hpp"
#include "parameter.hpp"
#include "ticket.hpp"
#include "loginList.hpp"
#include "waitingList.hpp"

class TrainSystem;

struct CompareTrainIndex;

class TrainIndex {
    TrainID ID;
    bool released = false;
    friend TrainSystem;
    friend CompareTrainIndex;
public:
    TrainIndex() = default;

    explicit TrainIndex(const TrainID &trainID) : ID(trainID) {}

    TrainIndex(const TrainID &trainID, const bool &released) : ID(trainID), released(released) {}

    friend bool operator<(const TrainIndex &a, const TrainIndex &b);

    friend bool operator==(const TrainIndex &a, const TrainIndex &b);

};


bool operator<(const TrainIndex &a, const TrainIndex &b) {
    return a.ID < b.ID;
}

bool operator==(const TrainIndex &a, const TrainIndex &b) {
    return (a.ID == b.ID) && (a.released == b.released);
}

struct CompareTrainIndex {
    bool operator()(const TrainIndex &a, const TrainIndex &b) {
        return a.ID < b.ID;
    }
};

/*
 * manage operations about train information
 * including add, query, modify, delete
 */
class TrainSystem {
    /*
     * Key: TrainID
     * Value: Train
     * store basic information and some address
     */
    BPlusTree<TrainIndex, Train, CompareTrainIndex, CompareTrainIndex, CompareTrainIndex> trainTree{"train_tree"};

    friend TransactionSystem;

    //return start sale
    static Time FirstTime(const std::string &sale_date);

    //return end sale
    static Time SecondTime(const std::string &sale_date);

    /*
     * add_train
     * return address of Station storing information of the first one
     */
    static long AddStation(const std::string &stations,
                           const std::string &prices,
                           const std::string &travel_times,
                           const std::string &stop_over_times,
                           const int &station_num,
                           const Interval &start_time,
                           FileManager<Station> &stationFile);

    /*
     * add_train
     * return address of first seat(int)
     */
    static long AddSeat(const int &seat_num, const int &station_num, const int &day, FileManager<Seat> &seatFile);

    /*
     * query_train
     * print train information on given date
     * return false if failed
     */
    static bool PrintTrainInformation(const long &train_addr,
                                      const std::string &trainID,
                                      const Time &date,
                                      FileManager<Train> &trainFile,
                                      FileManager<Station> &stationFile,
                                      FileManager<Seat> &seatFile);

    /*
     * buy_ticket
     * in stationFile
     * find index of station by traverse
     * return price_sum if succeed
     * return -1 if station not exist
     */
    int GetStationIndex(char *from, char *to, int &start, int &end);

    /*
     * buy_ticket
     * in seatFile
     * buy ticket from station A to B on given date
     * if succeed return number of ticket
     * if failed
     *     enqueue(to waitingList and modify seatFile) or fail
     */


    /*
     * buy_ticket
     * check if date is valid
     * find station
     * find ticket
     * try t buy ticket
     */
    void BuyTicket(const Train &train,
                   const Time &time,
                   const int &number,
                   char *from, char *to,
                   const bool &flag,
                   TransactionSystem &transactionSystem,
                   WaitingList &waitingList,
                   FileManager<Station> &stationFile,
                   FileManager<Seat> &seatFile,
                   FileManager<WaitingTransaction> &waitingListFile);

public:
    /*
     * add_train
     * return 0 if succeed
     * return -1 if failed
     */
    int AddTrain(const Parameter &parameter,
                 FileManager<Train> &trainFile, FileManager<Station> &stationFile, FileManager<Seat> &seatFile);

    /*
     * delete_train
     * delete unreleased train
     * return 0 if succeed
     * return -1 if failed
     */
    int DeleteTrain(const Parameter &parameter);

    /*
     * release_train
     * if not found or invalid return -1
     * else modify train.released, add into releasedTree, add tickets to ticketSystem return 0
     */
    int ReleaseTrain(const Parameter &parameter,
                     TicketSystem &ticketSystem,
                     FileManager<Train> &trainFile,
                     FileManager<Station> &stationFile,
                     FileManager<Seat> &seatFile);

    /*
     * query_train
     * print all the information
     * return -1 if failed
     */
    void QueryTrain(const Parameter &parameter,
                    FileManager<Train> &trainFile,
                    FileManager<Station> &stationFile,
                    FileManager<Seat> &seatFile);

    /*
     * buy_ticket
     * leave from on given date
     */
    void BuyTicket(const Parameter &parameter,
                   LoginList &loginList,
                   TransactionSystem &transactionSystem,
                   WaitingList &waitingList,
                   FileManager<Train> &trainFile,
                   FileManager<Station> &stationFile,
                   FileManager<Seat> &seatFile,
                   FileManager<WaitingTransaction> &waitingListFile);

};

/*
 * PRIVATE
 * -----------------------------------------------------------------------------------------------------------------------------------------------------------------------
 */

Time TrainSystem::FirstTime(const std::string &sale_date) {
    int para1 = (sale_date[0] - '0') * 10 + sale_date[1] - '0';
    int para2 = (sale_date[3] - '0') * 10 + sale_date[4] - '0';
    if (para1 == 8) para2 += 61;
    else if (para1 == 7) para2 += 30;
    --para2;
    return Time(para2 * 1440);
}

Time TrainSystem::SecondTime(const std::string &sale_date) {
    int para1 = (sale_date[6] - '0') * 10 + sale_date[7] - '0';
    int para2 = (sale_date[9] - '0') * 10 + sale_date[10] - '0';
    if (para1 == 8) para2 += 61;
    else if (para1 == 7) para2 += 30;
    --para2;
    return Time(para2 * 1440);
}

long TrainSystem::AddStation(const std::string &stations,
                             const std::string &prices,
                             const std::string &travel_times,
                             const std::string &stop_over_times,
                             const int &station_num,
                             const Interval &start_time,
                             FileManager<Station> &stationFile) {
    Scanner<std::string> scan_station(stations);
    Scanner<int> scan_price(prices);
    Scanner<int> scan_travel_time(travel_times);
    Scanner<int> scan_stop_over_time(stop_over_times);
    std::string name;
    int price, travel_time, stop_over_time;
    Interval arriving_time(start_time), leaving_time(start_time);
    //the first station
    scan_station.GetStr(name);
    stationFile.WriteEle(Station(name, 0, arriving_time, leaving_time));
    for (int i = 2; i < station_num; ++i) {
        scan_station.GetStr(name);
        scan_price.GetNum(price);
        scan_travel_time.GetNum(travel_time);
        scan_stop_over_time.GetNum(stop_over_time);
        arriving_time = leaving_time + travel_time;
        leaving_time += stop_over_time;
        stationFile.WriteEle(Station(name, price, arriving_time, leaving_time));
    }
    //the last station
    scan_station.GetStr(name);
    scan_price.GetNum(price);
    scan_travel_time.GetNum(travel_time);
    arriving_time = leaving_time + travel_time;
    stationFile.WriteEle(Station(name, price, arriving_time, leaving_time));
}

long TrainSystem::AddSeat(const int &seat_num, const int &station_num, const int &day, FileManager<Seat> &seatFile) {
    seatFile.WriteEle(Seat(seat_num), day * (station_num - 1));
}

bool TrainSystem::PrintTrainInformation(const long &train_addr,
                                        const std::string &trainID,
                                        const Time &date,
                                        FileManager<Train> &trainFile,
                                        FileManager<Station> &stationFile,
                                        FileManager<Seat> &seatFile) {
    //read train
    Train train;
    trainFile.ReadEle(train_addr, train);
    //invalid time request
    if (date < train.start_sale || train.stop_sale < date)return false;
    std::cout << trainID << ' ' << train.type;
    Station station;
    Seat seat;
    long station_address = train.station_addr, seat_address = train.seat_addr;
    int price_sum = 0;
    //first station
    std::cout << '\n';
    stationFile.ReadEle(station_address, station);
    std::cout << station.name << " xx-xx xx:xx -> " << (date + station.leaving_time) << ' ' << price_sum << ' ';
    seatFile.ReadEle(seat_address, seat);
    std::cout << seat.num;
    for (int i = 1; i < train.stationNum - 1; ++i) {
        std::cout << '\n';
        stationFile.ReadEle(station_address, i, station);
        price_sum += station.price;
        std::cout << station.name << ' ' << (date + station.arriving_time) << " -> " << (date + station.leaving_time)
                  << ' ' << price_sum << ' ';
        seatFile.ReadEle(seat_address, i, seat);
        std::cout << seat.num;
    }
    //last station
    std::cout << '\n';
    stationFile.ReadEle(station_address, train.stationNum - 1, station);
    price_sum += station.price;
    std::cout << station.name << ' ' << (date + station.arriving_time) << " -> xx-xx xx:xx " << price_sum << " x";
    return true;
}

/*
 * PUBLIC
 * -----------------------------------------------------------------------------------------------------------------------------------------------------------------
 */
int TrainSystem::AddTrain(const Parameter &parameter,
                          FileManager<Train> &trainFile,
                          FileManager<Station> &stationFile,
                          FileManager<Seat> &seatFile) {
    //check parameter
    std::string trainID, stations, prices, stopover_times, travel_times, start_time, sale_date;
    int station_num, seat_num;
    char type;
    if (!parameter.GetParameter('i', trainID) ||
        !parameter.GetParameter('n', station_num) ||
        !parameter.GetParameter('m', seat_num) ||
        !parameter.GetParameter('s', stations) ||
        !parameter.GetParameter('p', prices) ||
        !parameter.GetParameter('x', start_time) ||
        !parameter.GetParameter('t', travel_times) ||
        !parameter.GetParameter('o', stopover_times) ||
        !parameter.GetParameter('d', sale_date) ||
        !parameter.GetParameter('y', type))
        return -1;
    //try to insert
    Time start_sale = FirstTime(sale_date);
    Time stop_sale = SecondTime(sale_date);
    if (trainTree.Insert(TrainIndex(TrainID(trainID)),
                         Train(station_num, start_sale, stop_sale,
                               type, stationFile.GetAddress(), seatFile.GetAddress()),
                         trainFile)) {
        //if succeeded, write in stationFile and seatFile
        AddStation(stations, prices, travel_times, stopover_times, station_num, Interval(start_time), stationFile);
        AddSeat(seat_num, station_num, stop_sale - start_sale, seatFile);
    } else return -1;
}

int TrainSystem::DeleteTrain(const Parameter &parameter) {
    std::string trainID;
    if (!parameter.GetParameter('i', trainID)) return -1;
    if (trainTree.Delete(TrainIndex(TrainID(trainID)))) return 0;//if released key!=
    return -1;
}

int TrainSystem::ReleaseTrain(const Parameter &parameter,
                              TicketSystem &ticketSystem,
                              FileManager<Train> &trainFile,
                              FileManager<Station> &stationFile,
                              FileManager<Seat> &seatFile) {
    std::string trainID;
    if (!parameter.GetParameter('i', trainID)) return -1;
    long block_addr = 0;
    int ele_index = 0;
    TrainIndex train_index((TrainID(trainID)));
    long train_addr = trainTree.StrictFind(train_index, block_addr, ele_index);
    if (train_addr < 0) return -1;//not exist or released
    //update on bpt
    train_index.released = true;
    trainTree.RewriteKey(train_index, block_addr, ele_index);
    //release
    ticketSystem.ReleaseTrain(train_addr, trainFile, stationFile, seatFile);
}

void TrainSystem::QueryTrain(const Parameter &parameter,
                             FileManager<Train> &trainFile,
                             FileManager<Station> &stationFile,
                             FileManager<Seat> &seatFile) {
    std::string trainID, date;
    if (!parameter.GetParameter('i', trainID) ||
        !parameter.GetParameter('d', date)) {
        std::cout << -1;
        return;
    }
    TrainIndex train_index((TrainID(trainID)));
    sjtu::vector<long> vec = trainTree.StrictFind(train_index);
    if (vec.empty()) {//not exist
        std::cout << -1;
        return;
    }
    if (PrintTrainInformation(vec.front(), trainID, Time(date), trainFile, stationFile, seatFile))return;
    std::cout << -1;
}

void TrainSystem::BuyTicket(const Parameter &parameter,
                            LoginList &loginList,
                            TransactionSystem &transactionSystem,
                            WaitingList &waitingList,
                            FileManager<Train> &trainFile,
                            FileManager<Station> &stationFile,
                            FileManager<Seat> &seatFile,
                            FileManager<WaitingTransaction> &waitingListFile) {
    //check parameter
    std::string username, trainID, date, flag;
    char from[31], to[31];
    int number;
    if (!parameter.GetParameter('u', username) ||
        !parameter.GetParameter('i', trainID) ||
        !parameter.GetParameter('d', date) ||
        !parameter.GetParameter('n', number) ||
        !parameter.GetParameter('f', from) ||
        !parameter.GetParameter('t', to)) {
        std::cout << -1;
        return;
    }
    parameter.GetParameter('q', flag);
    bool enqueue_flag = (flag == "true");
    //check if logged in
    if (loginList.CheckLoggedIn(Username(username)) < 0) {
        std::cout << -1;
        return;
    }
    //check if released
    TrainIndex train_index(TrainID(trainID), true);
    sjtu::vector<long> vec = trainTree.StrictFind(train_index);
    if (vec.empty()) {//not exist or not released
        std::cout << -1;
        return;
    }
    //read train
    Train train;
    trainFile.ReadEle(vec.front(), train);
    Time date_(date);
    BuyTicket(train, date_, number, from, to, enqueue_flag,
              transactionSystem, waitingList, stationFile, seatFile, waitingListFile);
}


#endif //TICKETSYSTEM_TRAIN_HPP
