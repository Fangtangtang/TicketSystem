/*
 * TICKET SYSTEM
 * manage information of released train
 * deal with cmd
 *         release_train
 *         query_ticket
 *         query_transfer
 */

#ifndef TICKETSYSTEM_TICKET_HPP
#define TICKETSYSTEM_TICKET_HPP

#include "../utility/bpt.hpp"
#include "../utility/vector.hpp"
#include "../utility/pair.hpp"
#include "../utility/tool.hpp"
#include "../utility/file_manager.hpp"
#include "parameter.hpp"


class TimeBased;

class CostBased;

class TicketSystem;

class TicketDetail {
    TrainID trainID;
    int station_num = 0;//train's stationNum, used to cal move
    int station_interval = 0;//end-start
    int price = 0;
    int time = 0;
    Time leaving_time;//leaving time of the train set off on first day
    long seat_addr = 0;//start_addr of the first station's seat on start_sale day

    friend TimeBased;
    friend CostBased;
    friend TicketSystem;
public:
    TicketDetail() = default;

    TicketDetail(const TrainID &trainID_,
                 const int &station_num_,
                 const int &station_interval_,
                 const int &price_, const int &time_,
                 const Time &leaving_time_,
                 const long &seat_addr_);

};

TicketDetail::TicketDetail(const TrainID &trainID_,
                           const int &station_num_,
                           const int &station_interval_,
                           const int &price_, const int &time_,
                           const Time &leaving_time_,
                           const long &seat_addr_) :
        trainID(trainID_), station_num(station_num_), station_interval(station_interval_), price(price_), time(time_),
        leaving_time(leaving_time_),
        seat_addr(seat_addr_) {}


struct TimeBased {
    bool operator()(const TicketDetail &a, const TicketDetail &b) const;
};

bool TimeBased::operator()(const TicketDetail &a, const TicketDetail &b) const {
    if (a.time != b.time) return a.time < b.time;
    return a.trainID < b.trainID;
}

const TimeBased timeBased;

struct CostBased {
    bool operator()(const TicketDetail &a, const TicketDetail &b) const;
};

bool CostBased::operator()(const TicketDetail &a, const TicketDetail &b) const {
    if (a.price != b.price) return a.price < b.price;
    return a.trainID < b.trainID;
}

class CostBased costBased;

/*
 * manage operations about ticket information
 * including add, query
 */
class TicketSystem {
    BPlusTree<Ticket, TicketDetail> ticketTree{"ticket_tree"};

    friend TrainSystem;

    /*
     * release_train
     * provide from and to
     * calculate start_sale and stop_sale of the station
     * (given start_sale and stop_sale of train)
     */
    void AddTicket(const Station &from, const Station &to,
                   const Time &start_sale, const Time &stop_sale,
                   const TrainID &trainID, const int &station_num,
                   const int &station_interval, const long &seat_addr,
                   FileManager<TicketDetail> &ticketFile);

    /*
     * query_ticket\query_transfer
     * read from file
     */
    static void FindTicket(const sjtu::vector<long> &vec, const int &size,
                           FileManager<TicketDetail> &ticketFile,
                           sjtu::vector<TicketDetail> &tickets);

    /*
     * query_ticket\query_transfer
     * print ticket information
     */
    static void PrintTicket(const sjtu::vector<TicketDetail> &ticket_vec,
                            const std::string &from, const std::string &to,
                            const int &size, FileManager<Seat> &seatFile);

public:
    /*
     * release_train
     * store ticket information of the train to file
     * cout<<0 in the end
     */
    void ReleaseTrain(const TrainID &trainID,
                      const long &train_addr,
                      FileManager<Train> &trainFile,
                      FileManager<Station> &stationFile,
                      FileManager<Seat> &seatFile,
                      FileManager<TicketDetail> &ticketFile);

    /*
     * query_ticket
     * given date from to
     * print based on time or cost
     */
    void QueryTicket(const Parameter &parameter,
                     FileManager<TicketDetail> &ticketFile,
                     FileManager<Seat> &seatFile);

    /*
     * query_transfer
     * given date from to
     * need to transfer
     * return best choice based on time or cost
     */
    void QueryTransfer(const Parameter &parameter);


};

/*
 * PRIVATE
 * -----------------------------------------------------------------------------------------------------------------------------------------------------------------
 */
void TicketSystem::AddTicket(const Station &from, const Station &to,
                             const Time &start_sale, const Time &stop_sale,
                             const TrainID &trainID, const int &station_num,
                             const int &station_interval, const long &seat_addr,
                             FileManager<TicketDetail> &ticketFile) {
    //cal ticket_start_sale ticket_stop_sale
    Time ticket_start_sale = start_sale + from.leaving_time;
    Time ticket_stop_sale = stop_sale + from.leaving_time;
    ticketTree.Insert(Ticket(from.name, to.name, ticket_start_sale, ticket_stop_sale),
                      TicketDetail(trainID, station_num, station_interval,
                                   to.price - from.price, to.arriving_time - from.leaving_time,
                                   ticket_start_sale, seat_addr),
                      ticketFile, compareTicket);
}

void TicketSystem::FindTicket(const sjtu::vector<long> &vec, const int &size,
                              FileManager<TicketDetail> &ticketFile,
                              sjtu::vector<TicketDetail> &tickets) {
    TicketDetail ticketDetail;
    for (int i = 0; i < size; ++i) {
        ticketFile.ReadEle(vec[i], ticketDetail);
        tickets.push_back(ticketDetail);
    }
}

void TicketSystem::PrintTicket(const sjtu::vector<TicketDetail> &ticket_vec,
                               const std::string &from, const std::string &to,
                               const int &size, FileManager<Seat> &seatFile) {
    for (int i = 0; i < size; ++i) {
        std::cout << '\n';
        std::cout << ticket_vec[i].trainID << ' '
                  << from << ' ' << ticket_vec[i].leaving_time << " -> "
                  << to << ' ' << (ticket_vec[i].leaving_time + ticket_vec[i].time) << ' '
                  << ticket_vec[i].price << ' '
                  << seatFile.MinValue(ticket_vec[i].seat_addr, 0, ticket_vec[i].station_interval);
    }
}

/*
 * PUBLIC
 * -----------------------------------------------------------------------------------------------------------------------------------------------------------------
 */
void TicketSystem::ReleaseTrain(const TrainID &trainID,
                                const long &train_addr,
                                FileManager<Train> &trainFile,
                                FileManager<Station> &stationFile,
                                FileManager<Seat> &seatFile,
                                FileManager<TicketDetail> &ticketFile) {
    //read train
    Train train;
    trainFile.ReadEle(train_addr, train);
    //read stations
    sjtu::vector<Station> station_vec;
    stationFile.ReadEle(train.station_addr, train.stationNum, station_vec);
    //add every ticket
    for (int start = 0; start < train.stationNum; ++start) {
        for (int end = start + 1; end <= train.stationNum; ++end) {
            AddTicket(station_vec[start], station_vec[end],
                      train.start_sale, train.stop_sale,
                      trainID, train.stationNum, end - start,
                      seatFile.GetAddress(train.seat_addr, end - start),
                      ticketFile);
        }
    }
    std::cout << 0;
}

void TicketSystem::QueryTicket(const Parameter &parameter,
                               FileManager<TicketDetail> &ticketFile,
                               FileManager<Seat> &seatFile) {
    //check parameter
    std::string from, to, date, keyword;
    if (!parameter.GetParameter('s', from) ||
        !parameter.GetParameter('t', to) ||
        !parameter.GetParameter('d', date))
        return;
    parameter.GetParameter('p', keyword);
    Time time(date);//requested date
    bool flag = (keyword == "cost");//true if sort based on cost
    //find ticket from ... to ...
    sjtu::vector<long> vec;
    ticketTree.Find(Ticket(from, to, time, time),
                    compareTicketStopSale, isAvailableTicket,
                    vec);
    sjtu::vector<TicketDetail> ticket_vec;
    int number = vec.size();
    FindTicket(vec, number, ticketFile, ticket_vec);
    std::cout << number;
    //sort
    if (flag) {//sort based on cost
        sjtu::Sort(ticket_vec, 0, number - 1, costBased);
    } else {//sort based on time
        sjtu::Sort(ticket_vec, 0, number - 1, timeBased);
    }
    //print
    PrintTicket(ticket_vec, from, to, number, seatFile);
}


#endif //TICKETSYSTEM_TICKET_HPP
