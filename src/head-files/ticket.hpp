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
#include <utility>

#include "../utility/BPlusTree.hpp"
#include "../utility/map.hpp"
#include "../utility/vector.hpp"
#include "../utility/pair.hpp"
#include "../utility/tool.hpp"
#include "../utility/file_manager.hpp"
#include "parameter.hpp"

/*
 * TransferTicket class
 * a special kind of Ticket
 * add travel_time
 */


class TransferTicket : public Ticket {
    int travel_time = 0;
    
    friend TicketSystem;
    friend CompareTicket;
    friend CompareFrom;
    friend CompareTo;
    friend CompareTime;
    friend CompareTicketStopSale;
    friend CompareFromTicket;
    friend CompareToTicket;
    friend CompareTimeTicket;
    friend IsAvailableTicket;
    friend IsAvailableFrom;
    friend IsAvailableTo;
    friend IsAvailableTime;
public:
    TransferTicket() = default;

    TransferTicket(const Ticket &ticket, const int &travel_time_) : Ticket(ticket), travel_time(travel_time_) {}
};

/*
 * Compare class for TransferTicket
 * ==========================================================
 * //TODO
 * compare time to be modified
 * CompareFrom:
 *     used to insert in real from bpt
 *     ---------------------
 *     this tree is used to find where can go 
 *     at the given day from the given place 
 *     ---------------------
 *     from stop_sale start_sale to travel_time
 *
 * CompareTo:
 *     used to insert in real to bpt
 *     ---------------------
 *     this tree is used to find where can from 
 *     after the given day to the given place
 *     ---------------------
 *     to stop_sale start_sale from travel_time
 *     
 * CompareTime:
 *     used to insert in real time bpt
 *     ---------------------
 *     this tree is used to find 
 *     which tickets are available on the given day
 *     ---------------------
 *     stop_sale start_sale to from *     
 *     
 * 
 * CompareFromTicket:
 *     from stop_sale(cmp1 of fromTicketTree)
 *     
 * CompareToTicket:
 *     to stop_sale(cmp1 of toTicketTree)
 *     
 * CompareToTicket:
 *     stop_sale(cmp1 of timeTicketTree)
 *     
 * IsAvailableFrom:
 *     == (cmp2 of fromTicketTree)
 * 
 * IsAvailableTo:
 *     == (cmp2 of to/ticketTree)
 *     
 * IsAvailableTime:
 *     == (cmp2 of to/ticketTree)    
 *
 */

struct CompareFrom {
    bool operator()(const TransferTicket &a, const TransferTicket &b) const;
};

bool CompareFrom::operator()(const TransferTicket &a, const TransferTicket &b) const {
    int cmp = strcmp(a.from, b.from);
    if (cmp) return cmp < 0;
    if (!(a.stop_sale == b.stop_sale)) return a.stop_sale < b.stop_sale;
    if (!(a.start_sale == b.start_sale))return a.start_sale < b.start_sale;
    cmp = strcmp(a.to, b.to);
    if (cmp) return cmp < 0;
    return a.travel_time < b.travel_time;

}

const CompareFrom compareFrom;

struct CompareTo {
    bool operator()(const TransferTicket &a, const TransferTicket &b) const;
};

bool CompareTo::operator()(const TransferTicket &a, const TransferTicket &b) const {
    int cmp = strcmp(a.to, b.to);
    if (cmp) return cmp < 0;
    if (!(a.stop_sale == b.stop_sale)) return a.stop_sale < b.stop_sale;
    if (!(a.start_sale == b.start_sale))return a.start_sale < b.start_sale;
    cmp = strcmp(a.from, b.from);
    if (cmp) return cmp < 0;
    return a.travel_time < b.travel_time;
}

const CompareTo compareTo;

struct CompareTime {
    bool operator()(const TransferTicket &a, const TransferTicket &b) const;
};

bool CompareTime::operator()(const TransferTicket &a, const TransferTicket &b) const {
    if (!(a.stop_sale == b.stop_sale)) return a.stop_sale < b.stop_sale;
    if (!(a.start_sale == b.start_sale))return a.start_sale < b.start_sale;
    int cmp = strcmp(a.to, b.to);
    if (cmp) return cmp < 0;
    return strcmp(a.from, b.from);
}

const CompareTime compareTime;

struct CompareFromTicket {
    bool operator()(const TransferTicket &a, const TransferTicket &b) const;
};

bool CompareFromTicket::operator()(const TransferTicket &a, const TransferTicket &b) const {
    int cmp = strcmp(a.from, b.from);
    if (cmp) return cmp < 0;
    return a.stop_sale < b.stop_sale;
}

const CompareFromTicket compareFromTicket;


struct CompareToTicket {
    bool operator()(const TransferTicket &a, const TransferTicket &b) const;
};

//TODO
//modify?
bool CompareToTicket::operator()(const TransferTicket &a, const TransferTicket &b) const {
    int cmp = strcmp(a.to, b.to);
    if (cmp) return cmp < 0;
    return a.stop_sale < b.stop_sale;
}

const CompareToTicket compareToTicket;

struct CompareTimeTicket {
    bool operator()(const TransferTicket &a, const TransferTicket &b) const;
};

bool CompareTimeTicket::operator()(const TransferTicket &a, const TransferTicket &b) const {
    return a.stop_sale < b.stop_sale;
}

const CompareTimeTicket compareTimeTicket;

struct IsAvailableFrom {
    bool operator()(const TransferTicket &a, const TransferTicket &b) const;
};

//a:target
bool IsAvailableFrom::operator()(const TransferTicket &a, const TransferTicket &b) const {
    if (strcmp(a.from, b.from)) return false;
    if (a.stop_sale < b.start_sale) return false;
    return true;
}

const IsAvailableFrom isAvailableFrom;

struct IsAvailableTo {
    bool operator()(const TransferTicket &a, const TransferTicket &b) const;
};

//a:target
bool IsAvailableTo::operator()(const TransferTicket &a, const TransferTicket &b) const {
    if (strcmp(a.to, b.to)) return false;
    if (b.stop_sale < a.stop_sale) return false;
    return true;
}

const IsAvailableTo isAvailableTo;

struct IsAvailableTime {
    bool operator()(const TransferTicket &a, const TransferTicket &b) const;
};

//a:target
bool IsAvailableTime::operator()(const TransferTicket &a, const TransferTicket &b) const {
    if (a.stop_sale < b.start_sale) return false;
    return true;
}

const IsAvailableTime isAvailableTime;

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

class Option {
    int time = 0;//cost or time based on -p
    int cost = 0;
    std::string from, transfer, to;
    TicketDetail ticket1, ticket2;
    friend TimeBased;
    friend CostBased;
public:
    Option() = default;

    Option(const int &time_, const int &cost_,
           std::string from_, std::string transfer_, std::string to_,
           const TicketDetail &ticketDetail1, const TicketDetail &ticketDetail2) :
            time(time_), cost(cost_), from(std::move(from_)), transfer(std::move(transfer_)), to(std::move(to_)),
            ticket1(ticketDetail1), ticket2(ticketDetail2) {}
};

struct TimeBased {
    bool operator()(const TicketDetail &a, const TicketDetail &b) const;

    bool operator()(const Option &a, const Option &b) const;
};

bool TimeBased::operator()(const TicketDetail &a, const TicketDetail &b) const {
    if (a.time != b.time) return a.time < b.time;
    return a.trainID < b.trainID;
}

bool TimeBased::operator()(const Option &a, const Option &b) const {
    if (a.time != b.time)return a.time < b.time;
    if (a.cost != b.cost) return a.cost < b.cost;
    if (!(a.ticket1.trainID == b.ticket1.trainID)) return a.ticket1.trainID < b.ticket1.trainID;
    return a.ticket2.trainID < b.ticket2.trainID;
}

const TimeBased timeBased;

struct CostBased {
    bool operator()(const TicketDetail &a, const TicketDetail &b) const;

    bool operator()(const Option &a, const Option &b) const;

};

bool CostBased::operator()(const TicketDetail &a, const TicketDetail &b) const {
    if (a.price != b.price) return a.price < b.price;
    return a.trainID < b.trainID;
}

bool CostBased::operator()(const Option &a, const Option &b) const {
    if (a.cost != b.cost)return a.cost < b.cost;
    if (a.time != b.time) return a.time < b.time;
    if (!(a.ticket1.trainID == b.ticket1.trainID)) return a.ticket1.trainID < b.ticket1.trainID;
    return a.ticket2.trainID < b.ticket2.trainID;
}

class CostBased costBased;

/*
 * manage operations about ticket information
 * including add, query
 */
class TicketSystem {
    BPlusIndexTree<Ticket, TicketDetail> ticketTree{"ticket_tree"};

    BPlusTree<TransferTicket, long> fromTicketTree{"from_ticket_tree", "from_ticket_list"};//real BPT
    BPlusTree<TransferTicket, long> toTicketTree{"to_ticket_tree", "to_ticket_list"};//real BPT
//    BPlusTree<Ticket, long> timeTicketTree{"time_ticket_tree", "time_ticket_list"};//real BPT

    friend TrainSystem;

    struct TicketInf1 {
        Time leaving_time;
        Time arriving_time;
        long ticket_addr = 0;

        TicketInf1() = default;

        TicketInf1(const Time &leaving, const Time &arriving, const long &address) : leaving_time(leaving),
                                                                                     arriving_time(arriving),
                                                                                     ticket_addr(address) {}
    };

    struct TicketInf2 {
        Time start_sale;
        Time stop_sale;
        int travel_time = 0;
        long ticket_addr = 0;

        TicketInf2() = default;

        TicketInf2(const Time &start, const Time &stop,
                   const int &traval_time_, const long &address) :
                start_sale(start), stop_sale(stop), travel_time(traval_time_), ticket_addr(address) {}
    };

    struct AddressSet {
        sjtu::vector<TicketInf1> from_transfer;
        sjtu::vector<TicketInf2> transfer_to;
    };


    /*
     * release_train
     * provide from and to
     * calculate start_sale and stop_sale of the station
     * (given start_sale and stop_sale of train)
     * add into two bpt
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
                            const Time &date,
                            const int &size, FileManager<Seat> &seatFile);

    /*
     * query_transfer
     * find ticket in the fromTree
     * from 'from' and time available
     * insert into map
     * ------------------------
     * what is in map:
     * vec1:
     * leaving time and arriving time of a ticket,
     * address of ticketDetail
     * -------------------------
     */
    void FindTransferFrom(sjtu::map<std::string, AddressSet> &map,
                          const Ticket &ticket);

    /*
     * query_transfer
     * find the ticket start from 'from'
     * add transfer station name as key
     * push address of the ticket into vector(one of the value)
     */
//    void FindTransfer(const sjtu::vector<sjtu::pair<Ticket, long>> &vec,
//                      sjtu::map<std::string, AddressSet> &map,
//                      const Ticket &ticket);

    /*
     * query_transfer
     * find the ticket start to 'to' available on or after given date
     * if vec1 in map[] not empty, possible transfer
     * push TicketInf2 into vec2
     */
    void FindTransferTo(sjtu::map<std::string, AddressSet> &map,
                        const Ticket &ticket);

    /*
     * query_transfer
     * map[],get possible transfer
     */
    void TryTransfer();

    void PrintOption(const Option &option, FileManager<Seat> &seatFile);

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
    void QueryTransfer(const Parameter &parameter,
                       FileManager<TicketDetail> &ticketFile,
                       FileManager<Seat> &seatFile);


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
    Ticket key(from.name, to.name, ticket_start_sale, ticket_stop_sale);
    int travel_time = to.arriving_time - from.leaving_time;
    ticketTree.Insert(key,
                      TicketDetail(trainID, station_num, station_interval,
                                   to.price - from.price, travel_time,
                                   ticket_start_sale, seat_addr),
                      ticketFile, compareTicket);
    TransferTicket transfer_ticket(key, travel_time);
    fromTicketTree.Insert(transfer_ticket, ticketFile.GetPreAddress(), compareFrom);
    toTicketTree.Insert(transfer_ticket, ticketFile.GetPreAddress(), compareTo);
//    timeTicketTree.Insert(key, ticketFile.GetPreAddress(), compareTime);
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
                               const Time &date,
                               const int &size, FileManager<Seat> &seatFile) {
    long address;
    for (int i = 0; i < size; ++i) {
        std::cout << '\n';
        address = ticket_vec[i].seat_addr +
                  ticket_vec[i].station_num * (date - ticket_vec[i].leaving_time) * sizeof(Seat);
        std::cout << ticket_vec[i].trainID << ' '
                  << from << ' ' << ticket_vec[i].leaving_time << " -> "
                  << to << ' ' << (ticket_vec[i].leaving_time + ticket_vec[i].time) << ' '
                  << ticket_vec[i].price << ' '
                  << seatFile.MinValue(address, 0, ticket_vec[i].station_interval);
    }
}

void TicketSystem::FindTransferFrom(sjtu::map<std::string, AddressSet> &map, const Ticket &ticket) {
    sjtu::vector<sjtu::pair<TransferTicket, long>> vec;
    TransferTicket transfer_ticket(ticket, 0);
    fromTicketTree.Find(transfer_ticket, compareFromTicket, isAvailableFrom, vec);
    int size = vec.size();
    for (int i = 0; i < size; ++i) {
        Time time = ticket.start_sale.Add(ticket.start_sale.Lag(vec[i].first.start_sale), 0);
        map[std::string(vec[i].first.to)].from_transfer.push_back(
                TicketInf1(time, time + vec[i].first.travel_time, vec[i].second));
    }
}

void TicketSystem::FindTransferTo(sjtu::map<std::string, AddressSet> &map, const Ticket &ticket) {
    sjtu::vector<sjtu::pair<TransferTicket, long>> vec;
    TransferTicket transfer_ticket(ticket, 0);
    toTicketTree.Find(transfer_ticket, compareToTicket, isAvailableTo, vec);
    int size = vec.size();
    for (int i = 0; i < size; ++i) {
        try {
            map.at(std::string(vec[i].first.to)).transfer_to.push_back(
                    TicketInf2(vec[i].first.start_sale, vec[i].first.stop_sale, vec[i].first.travel_time, vec[i].second)
            );
        } catch (...) {}//if vec1 not exist, throw
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
    PrintTicket(ticket_vec, from, to, time, number, seatFile);
}

void TicketSystem::QueryTransfer(const Parameter &parameter,
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
    //find available tickets on given day
    Ticket ticket(from, to, time, time);
    //find ticket start from the requested station (possible date)
    sjtu::map<std::string, AddressSet> map;
    FindTransferFrom(map, ticket);
    FindTransferTo(map, ticket);
    sjtu::vector<Option> option_vec;

    //TODO
    //print the best option
    PrintOption(option_vec.front(), seatFile);
}


#endif //TICKETSYSTEM_TICKET_HPP
