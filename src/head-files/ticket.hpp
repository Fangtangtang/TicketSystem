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

class TicketDetail {
    TrainID trainID;
    int station_num;//train's stationNum, used to cal move
    int station_interval;//end-start
    int price = 0;
    int time = 0;
    Time leaving_time;//leaving time of the train set off on first day
    long seat_addr = 0;//start_addr of the first station's seat on start_sale day

    friend TimeBased;
    friend CostBased;
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
    bool operator()(const TicketDetail &a, const TicketDetail &b);
};

bool TimeBased::operator()(const TicketDetail &a, const TicketDetail &b) {
    if (a.time != b.time) return a.time < b.time;
    return a.trainID < b.trainID;
}

struct CostBased {
    bool operator()(const TicketDetail &a, const TicketDetail &b);
};

bool CostBased::operator()(const TicketDetail &a, const TicketDetail &b) {
    if (a.price != b.price) return a.price < b.price;
    return a.trainID < b.trainID;
}

/*
 * manage operations about ticket information
 * including add, query
 */
class TicketSystem {
    BPlusTree<Ticket, TicketDetail, CompareStationName, CompareTicket2, CompareTicket2> ticketTree{"ticket_tree"};

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
     * check if date is available
     *
     */
    void FindTicket(const sjtu::vector<long> &vec,
                    FileManager<TicketDetail> &ticketFile,
                    const Time &date,
                    sjtu::vector<TicketDetail> &tickets);

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
    void QueryTicket(const Parameter &parameter);

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
                                   ticket_start_sale, seat_addr), ticketFile);
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

void TicketSystem::QueryTicket(const Parameter &parameter) {
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
    //TODO
    sjtu::vector<long> vec = ticketTree.StrictFind(Ticket(from, to, time, time));

}


#endif //TICKETSYSTEM_TICKET_HPP
