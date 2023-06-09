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
#include "transaction.hpp"

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

    friend std::ostream &operator<<(std::ostream &os, const TrainIndex &information) {
        os << information.ID << ' ';
        return os;
    }

};


bool operator<(const TrainIndex &a, const TrainIndex &b) {
    return a.ID < b.ID;
}

bool operator==(const TrainIndex &a, const TrainIndex &b) {
    return (a.ID == b.ID) && (a.released == b.released);
}

struct CompareTrainIndex {
    bool operator()(const TrainIndex &a, const TrainIndex &b) const {
        return a.ID < b.ID;
    }
};

const CompareTrainIndex compareTrainIndex;

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
    BPlusIndexTree<TrainIndex, Train> trainTree{"train_tree"};

    friend TransactionSystem;

    //return start sale
    static Time FirstTime(const std::string &sale_date);

    //return end sale
    static Time SecondTime(const std::string &sale_date);

    /*
     * add_train
     * return address of Station storing information of the first one
     */
    static void AddStation(const std::string &stations,
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
    static void AddSeat(const int &seat_num, const int &station_num, const int &day, FileManager<Seat> &seatFile);

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
     * leaving_time and arriving_time needed(Internal)
     * return -1 if station not exist
     */
    static int GetStationIndex(char *from, char *to,
                               int &start, int &end,
                               Interval &leaving_time, Interval &arriving_time,
                               const int &station_num,
                               const long &station_addr, FileManager<Station> &stationFile);

    /*
     * buy_ticket
     * get vector of Seat intending to buy
     * return vector and min seat_num
     * if the same day, day == 0 -> move_base==0
     */
    static void GetSeat(const int &station_interval,//end-start
                        const long &start_addr, long &end_addr,
                        sjtu::vector<Seat> &vec, int &min_num,
                        FileManager<Seat> &seatFile);

    /*
     * buy_ticket
     * in seatFile
     * buy ticket from station A to B on given date
     */
    static void BuyTicket(const int &station_interval, const long &start_addr,
                          const sjtu::vector<Seat> &vec,
                          const int &number, FileManager<Seat> &seatFile);

    /*
     * buy_ticket
     * find station
     * find ticket
     * try to buy ticket
     * return true if in need of waiting
     */
    static bool BuyTicket(const Train &train,
                          const int &lag,
                          const int &number,
                          const int &start, const int &end, const int &price,
                          const bool &flag,
                          STATUS &status, long &start_addr, long &end_addr, long &train_on_day,
                          FileManager<Seat> &seatFile);

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
                     FileManager<Seat> &seatFile,
                     FileManager<TicketDetail> &ticketFile);

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
                   FileManager<TransactionDetail> &transactionFile,
                   FileManager<WaitingOrder> &waitingListFile);

    void Clean();
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

void TrainSystem::AddStation(const std::string &stations,
                             const std::string &prices,
                             const std::string &travel_times,
                             const std::string &stop_over_times,
                             const int &station_num,
                             const Interval &start_time,//set off at what time that day
                             FileManager<Station> &stationFile) {
    Scanner<std::string> scan_station(stations);
    Scanner<int> scan_price(prices);
    Scanner<int> scan_travel_time(travel_times);
    Scanner<int> scan_stop_over_time(stop_over_times);
    std::string name;
    int price, sum = 0, travel_time, stop_over_time;
    Interval arriving_time(start_time), leaving_time(start_time);
    //the first station
    scan_station.GetStr(name);
    stationFile.WriteEle(Station(name, sum, arriving_time, leaving_time));
    for (int i = 2; i < station_num; ++i) {
        scan_station.GetStr(name);
        scan_price.GetNum(price);
        sum += price;
        scan_travel_time.GetNum(travel_time);
        scan_stop_over_time.GetNum(stop_over_time);
        arriving_time = leaving_time + travel_time;
        leaving_time = arriving_time + stop_over_time;
        stationFile.WriteEle(Station(name, sum, arriving_time, leaving_time));
    }
    //the last station
    scan_station.GetStr(name);
    scan_price.GetNum(price);
    sum += price;
    scan_travel_time.GetNum(travel_time);
    arriving_time = leaving_time + travel_time;
    stationFile.WriteEle(Station(name, sum, arriving_time, leaving_time));
}

void TrainSystem::AddSeat(const int &seat_num, const int &station_num, const int &day, FileManager<Seat> &seatFile) {
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
    long station_address = train.station_addr,
            seat_address = train.seat_addr + (train.stationNum - 1) * date.Lag(train.start_sale) * SEAT_SIZE;
    //first station
    std::cout << '\n';
    stationFile.ReadEle(station_address, station);
    std::cout << station.name << " xx-xx xx:xx -> " << (date + station.leaving_time) << ' ' << 0 << ' ';
    seatFile.ReadEle(seat_address, seat);
    std::cout << seat.num;
    for (int i = 1; i < train.stationNum - 1; ++i) {
        std::cout << '\n';
        stationFile.ReadEle(station_address, i, station);
        std::cout << station.name << ' ' << (date + station.arriving_time) << " -> " << (date + station.leaving_time)
                  << ' ' << station.price << ' ';
        seatFile.ReadEle(seat_address, i, seat);
        std::cout << seat.num;
    }
    //last station
    std::cout << '\n';
    stationFile.ReadEle(station_address, train.stationNum - 1, station);
    std::cout << station.name << ' ' << (date + station.arriving_time) << " -> xx-xx xx:xx " << station.price << " x";
    return true;
}

int TrainSystem::GetStationIndex(char *from, char *to,
                                 int &start, int &end,
                                 Interval &leaving_time, Interval &arriving_time,
                                 const int &station_num,
                                 const long &station_addr, FileManager<Station> &stationFile) {
    Station station;
    int i = 0;
    for (; i < station_num; ++i) {
        stationFile.ReadEle(station_addr, i, station);
        if (strcmp(station.name, from) == 0) break;
    }
    start = i;
    leaving_time = station.leaving_time;
    int price = -station.price;
    for (; i < station_num; ++i) {
        stationFile.ReadEle(station_addr, i, station);
        if (strcmp(station.name, to) == 0) break;
    }
    if (i == station_num) return -1;
    price += station.price;
    end = i;
    arriving_time = station.arriving_time;
    return price;
}

void TrainSystem::GetSeat(const int &station_interval,//end-start
                          const long &start_addr, long &end_addr,
                          sjtu::vector<Seat> &vec, int &min_num,
                          FileManager<Seat> &seatFile) {
    Seat seat;
    for (int i = 0; i < station_interval; ++i) {
        seatFile.ReadEle(start_addr, i, seat);
        min_num = std::min(min_num, seat.num);
        vec.push_back(seat);
    }
    end_addr = start_addr + (station_interval - 1) * SEAT_SIZE;//the last seat's addr
}

void TrainSystem::BuyTicket(const int &station_interval, const long &start_addr,
                            const sjtu::vector<Seat> &vec,
                            const int &number, FileManager<Seat> &seatFile) {
    Seat seat;
    for (int i = 0; i < station_interval; ++i) {
        seat = vec[i];
        seat.num -= number;
        seatFile.WriteEle(start_addr, i, seat);
    }
}

bool TrainSystem::BuyTicket(const Train &train, const int &lag, const int &number,
                            const int &start, const int &end, const int &price,
                            const bool &flag, STATUS &status, long &start_addr, long &end_addr, long &train_on_day,
                            FileManager<Seat> &seatFile) {
    //find seat intending to buy
    sjtu::vector<Seat> vec;
    int min_num = number;
    train_on_day = train.seat_addr + lag * (train.stationNum - 1) * SEAT_SIZE;
    start_addr = train_on_day + start * SEAT_SIZE;
    GetSeat(end - start, start_addr, end_addr, vec, min_num, seatFile);
    //try to buy
    if (min_num < number) {
        if (flag) {//waiting
            std::cout << "queue";
            status = pending;
            return true;
        } else {
            std::cout << -1;
            return false;
        }
    } else {//buy ticket
        BuyTicket(end - start, start_addr, vec, number, seatFile);
        std::cout << number * price;
        status = success;
        return true;
    }
    //record transaction
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
                         Train(station_num, seat_num, start_sale, stop_sale,
                               type, stationFile.GetAddress(), seatFile.GetAddress()),
                         trainFile, compareTrainIndex)) {
        //if succeeded, write in stationFile and seatFile
        AddStation(stations, prices, travel_times, stopover_times, station_num, Interval(start_time), stationFile);
        AddSeat(seat_num, station_num, stop_sale - start_sale, seatFile);
        return 0;
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
                              FileManager<Seat> &seatFile,
                              FileManager<TicketDetail> &ticketFile) {
    std::string trainID;
    if (!parameter.GetParameter('i', trainID)) return -1;
    long block_addr = 0;
    int ele_index = 0;
    TrainID ID(trainID);
    TrainIndex train_index(ID);
    long train_addr = trainTree.Find(train_index, block_addr, ele_index);
    if (train_addr < 0) return -1;//not exist or released
    //update on bpt
    train_index.released = true;
    trainTree.RewriteKey(train_index, block_addr, ele_index);
    //release
    ticketSystem.ReleaseTrain(ID, train_addr, trainFile, stationFile, seatFile, ticketFile);
    return 0;
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
    sjtu::vector<long> vec;
    trainTree.Find(train_index, compareTrainIndex, vec);
    if (vec.empty()) {//not exist
        std::cout << -1;
        return;
    }
    Time time(date);
    if (!time.Check()) {
        std::cout << -1;
        return;
    }
    if (PrintTrainInformation(vec.front(), trainID, time, trainFile, stationFile, seatFile))return;
    std::cout << -1;
}

void TrainSystem::BuyTicket(const Parameter &parameter,
                            LoginList &loginList,
                            TransactionSystem &transactionSystem,
                            WaitingList &waitingList,
                            FileManager<Train> &trainFile,
                            FileManager<Station> &stationFile,
                            FileManager<Seat> &seatFile,
                            FileManager<TransactionDetail> &transactionFile,
                            FileManager<WaitingOrder> &waitingListFile) {
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
    Username user(username);
    if (loginList.CheckLoggedIn(user) < 0) {
        std::cout << -1;
        return;
    }
    //check if released
    TrainID ID(trainID);
    TrainIndex train_index(ID, true);
    sjtu::vector<long> vec;
    trainTree.Find(train_index, compareTrainIndex, vec);
    if (vec.empty()) {//not exist or not released
        std::cout << -1;
        return;
    }
    //read train
    Train train;
    trainFile.ReadEle(vec.front(), train);
    Time date_(date);
    if (!date_.Check()) {
        std::cout << -1;
        return;
    }
    //check seat
    if (train.seat_num < number) {
        std::cout << -1;
        return;
    }
    Interval leaving_time, arriving_time;
    int start, end;
    int price = GetStationIndex(from, to, start, end, leaving_time, arriving_time,
                                train.stationNum, train.station_addr, stationFile);
    if (price < 0) {//invalid station
        std::cout << -1;
        return;
    }
    //check date
    Time leaving = train.start_sale + leaving_time;
    if (date_ < leaving || (train.stop_sale + leaving_time) < date_) {
        std::cout << -1;
        return;
    }
    //time lag between the leaving and date
    int lag = date_.Lag(leaving);
    leaving.AddDay(lag);
    Time arriving = train.start_sale + arriving_time;
    arriving.AddDay(lag);
    //really need to enqueue or not
    STATUS status;
    long start_addr, end_addr, train_on_day;
    if (!BuyTicket(train, lag, number, start, end,
                   price, enqueue_flag,
                   status, start_addr, end_addr, train_on_day, seatFile))
        return;
    //record transaction
    long transaction_addr = transactionSystem.AddTransaction(user, parameter.GetTimestamp(),
                                                             ID, from, to, leaving, arriving,
                                                             price, number, status, start_addr,
                                                             start, end,
                                                             transactionFile);
    //enqueue
    if (status == pending) {
        waitingList.StartWaiting(start, end, number, parameter.GetTimestamp(), train_on_day, transaction_addr,
                                 waitingListFile);
    }
}

void TrainSystem::Clean() {
    trainTree.Clean();
}


#endif //TICKETSYSTEM_TRAIN_HPP
