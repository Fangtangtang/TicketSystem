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
    int cost = 0;
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

    TransferTicket(const Ticket &ticket, const int &travel_time_, const int &cost_) : Ticket(ticket),
                                                                                      travel_time(travel_time_),
                                                                                      cost(cost_) {}
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
    return strcmp(a.from, b.from) < 0;
//    int cmp = strcmp(a.from, b.from);
//    if (cmp) return cmp < 0;
//    if (!(a.stop_sale == b.stop_sale)) return a.stop_sale < b.stop_sale;
//    if (!(a.start_sale == b.start_sale))return a.start_sale < b.start_sale;
//    cmp = strcmp(a.to, b.to);
//    if (cmp) return cmp < 0;
//    return a.travel_time < b.travel_time;

}

const CompareFrom compareFrom;

struct CompareTo {
    bool operator()(const TransferTicket &a, const TransferTicket &b) const;
};

bool CompareTo::operator()(const TransferTicket &a, const TransferTicket &b) const {
    return strcmp(a.to, b.to) < 0;
//    int cmp = strcmp(a.to, b.to);
//    if (cmp) return cmp < 0;
//    if (!(a.stop_sale == b.stop_sale)) return a.stop_sale < b.stop_sale;
//    if (!(a.start_sale == b.start_sale))return a.start_sale < b.start_sale;
//    cmp = strcmp(a.from, b.from);
//    if (cmp) return cmp < 0;
//    return a.travel_time < b.travel_time;
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
    return !compareExactTime(a.stop_sale, b.start_sale);
}

const IsAvailableFrom isAvailableFrom;

struct IsAvailableTo {
    bool operator()(const TransferTicket &a, const TransferTicket &b) const;
};

//a:target
bool IsAvailableTo::operator()(const TransferTicket &a, const TransferTicket &b) const {
    if (strcmp(a.to, b.to)) return false;
    return !compareExactTime(a.stop_sale, b.start_sale);
}

const IsAvailableTo isAvailableTo;

class TimeBased;

class CostBased;

class TicketSystem;

class TicketDetail {
    TrainID trainID;
    int station_num = 0;//train's stationNum, used to cal move
    int station_interval = 0;//end-start
    int price = 0;
    int time = 0;
    Time leaving_time;//leaving time of the train set off on first day(exact time)
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

    friend std::ostream &operator<<(std::ostream &os, const TicketDetail &information);

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

std::ostream &operator<<(std::ostream &os, const TicketDetail &information) {
    os << '\n';
    os << information.trainID << '\n';
    os << information.time << ' ' << information.price
       << ' ' << information.leaving_time << ' ' << information.station_interval << '\n';
    return os;
}

class OptionTicket;

class Option {
    int time = 0;//cost or time based on -p
    int cost = 0;
    std::string transfer;
    friend TimeBased;
    friend CostBased;
    friend TicketSystem;
    friend OptionTicket;
protected:
    long ticket1;
    long ticket2;
public:
    Option() = default;

    Option(const int &time, const int &cost) : time(time), cost(cost) {}

    Option(const int &time_, const int &cost_, std::string transfer_,
           const long &addr1, const long &addr2) :
            time(time_), cost(cost_), transfer(std::move(transfer_)),
            ticket1(addr1), ticket2(addr2) {}
};

class OptionTicket : public Option {
    TicketDetail ticketDetail1;
    TicketDetail ticketDetail2;
    friend TimeBased;
    friend CostBased;
    friend TicketSystem;
public:
    OptionTicket() = default;

    OptionTicket(const Option &option, FileManager<TicketDetail> &ticketFile) : Option(option) {
        ticketFile.ReadEle(option.ticket1, ticketDetail1);
        ticketFile.ReadEle(option.ticket2, ticketDetail2);
    }
};

struct TimeBased {
    bool operator()(const TicketDetail &a, const TicketDetail &b) const;

    bool operator()(const Option &a, const Option &b) const;

    bool operator()(const OptionTicket &a, const OptionTicket &b) const;

};

bool TimeBased::operator()(const TicketDetail &a, const TicketDetail &b) const {
    if (a.time != b.time) return a.time < b.time;
    return a.trainID < b.trainID;
}

bool TimeBased::operator()(const Option &a, const Option &b) const {
    if (a.time != b.time)return a.time < b.time;
    return a.cost < b.cost;
}

bool TimeBased::operator()(const OptionTicket &a, const OptionTicket &b) const {
    if (a.time != b.time)return a.time < b.time;
    if (a.cost != b.cost)return a.cost < b.cost;
    if (!(a.ticketDetail1.trainID == b.ticketDetail1.trainID))return a.ticketDetail1.trainID < b.ticketDetail1.trainID;
    return a.ticketDetail2.trainID < b.ticketDetail2.trainID;
}

const TimeBased timeBased;

struct CostBased {
    bool operator()(const TicketDetail &a, const TicketDetail &b) const;

    bool operator()(const Option &a, const Option &b) const;

    bool operator()(const OptionTicket &a, const OptionTicket &b) const;

};

bool CostBased::operator()(const TicketDetail &a, const TicketDetail &b) const {
    if (a.price != b.price) return a.price < b.price;
    return a.trainID < b.trainID;
}

bool CostBased::operator()(const Option &a, const Option &b) const {
    if (a.cost != b.cost)return a.cost < b.cost;
    return a.time < b.time;
}

bool CostBased::operator()(const OptionTicket &a, const OptionTicket &b) const {
    if (a.cost != b.cost)return a.cost < b.cost;
    if (a.time != b.time)return a.time < b.time;
    if (!(a.ticketDetail1.trainID == b.ticketDetail1.trainID))return a.ticketDetail1.trainID < b.ticketDetail1.trainID;
    return a.ticketDetail2.trainID < b.ticketDetail2.trainID;
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

    friend TrainSystem;

    struct TicketInf1 {
        int cost = 0;
        Time leaving_time;
        Time arriving_time;
        long ticket_addr = 0;

        TicketInf1() = default;

        TicketInf1(const int &cost_,
                   const Time &leaving, const Time &arriving,
                   const long &address) : cost(cost_),
                                          leaving_time(leaving),
                                          arriving_time(arriving),
                                          ticket_addr(address) {}
    };

    struct TicketInf2 {
        int cost = 0;
        Time start_sale;
        Time stop_sale;
        int travel_time = 0;
        long ticket_addr = 0;

        TicketInf2() = default;

        TicketInf2(const int &cost, const Time &start, const Time &stop,
                   const int &traval_time_, const long &address) :
                cost(cost), start_sale(start), stop_sale(stop),
                travel_time(traval_time_), ticket_addr(address) {}
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
    static void FindTicket(const Ticket &ticket, const sjtu::vector<sjtu::pair<Ticket, long>> &vec,
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
     * check if ticket1 and ticket2 can make up a from-transfer-to ticket
     * return option and flag
     */
    static sjtu::pair<Option, bool> CheckTransferTicket(const TicketInf1 &ticket1, const TicketInf2 &ticketInf2,
                                                        const std::string &transfer);

    /*
     * query_transfer
     * traverse map and turn ele of map into from-transfer-to ticket
     * find the top two (in case one is fake transfer) option based on keyword
     * in vec
     *     empty
     *     top one (fake or not)
     *     at least top two
     *     more with same time and cost
     */
    template<class Compare>
    void TryTransfer(sjtu::map<std::string, AddressSet> &map, const Compare &cmp,
                     sjtu::vector<Option> &option_vec);

    /*
     * check if the "best" is valid
     * print the valid one
     * if more than one, traverse to compare
     */
    template<class Compare>
    void PrintOption(const sjtu::vector<Option> &option_vec, const Compare &cmp, const Time &date,
                     const std::string &from, const std::string &to,
                     FileManager<TicketDetail> &ticketFile, FileManager<Seat> &seatFile);

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
    int price = to.price - from.price;
    ticketTree.Insert(key,
                      TicketDetail(trainID, station_num, station_interval,
                                   price, travel_time,
                                   ticket_start_sale, seat_addr),
                      ticketFile, compareTicket);
    TransferTicket transfer_ticket(key, travel_time, price);
    fromTicketTree.Insert(transfer_ticket, ticketFile.GetPreAddress(), compareFrom);
    toTicketTree.Insert(transfer_ticket, ticketFile.GetPreAddress(), compareTo);
//    timeTicketTree.Insert(key, ticketFile.GetPreAddress(), compareTime);
}

void TicketSystem::FindTicket(const Ticket &ticket, const sjtu::vector<sjtu::pair<Ticket, long>> &vec,
                              FileManager<TicketDetail> &ticketFile,
                              sjtu::vector<TicketDetail> &tickets) {
    TicketDetail ticketDetail;
    int size = vec.size();
    for (int i = 0; i < size; ++i) {
        if (isAvailableTicket(vec[i].first, ticket)) {
            ticketFile.ReadEle(vec[i].second, ticketDetail);
            tickets.push_back(ticketDetail);
        }
    }
}

void TicketSystem::PrintTicket(const sjtu::vector<TicketDetail> &ticket_vec,
                               const std::string &from, const std::string &to,
                               const Time &date,
                               const int &size, FileManager<Seat> &seatFile) {
    long address;
    for (int i = 0; i < size; ++i) {
        std::cout << '\n';
        int lag = date.Lag(ticket_vec[i].leaving_time);
        address = ticket_vec[i].seat_addr +//addr of first day
                  (ticket_vec[i].station_num - 1) * (lag) * SEAT_SIZE;//move to day
        Time leaving = ticket_vec[i].leaving_time.Add(lag, 0);
        std::cout << ticket_vec[i].trainID << ' '
                  << from << ' ' << leaving << " -> "
                  << to << ' ' << (leaving + ticket_vec[i].time) << ' '
                  << ticket_vec[i].price << ' '
                  << seatFile.MinValue(address, 0, ticket_vec[i].station_interval - 1);
    }
}

void TicketSystem::FindTransferFrom(sjtu::map<std::string, AddressSet> &map, const Ticket &ticket) {
    sjtu::vector<sjtu::pair<TransferTicket, long>> vec;
    TransferTicket transfer_ticket(ticket, 0, 0);
    fromTicketTree.Find(transfer_ticket, compareFrom, vec);
    int size = vec.size();
    for (int i = 0; i < size; ++i) {
        if (ticket.stop_sale < vec[i].first.start_sale || vec[i].first.stop_sale < ticket.start_sale) continue;
        Time time = vec[i].first.start_sale.Add(ticket.start_sale.Lag(vec[i].first.start_sale), 0);
        map[std::string(vec[i].first.to)].from_transfer.push_back(
                TicketInf1(vec[i].first.cost, time, time + vec[i].first.travel_time, vec[i].second));
    }
}

void TicketSystem::FindTransferTo(sjtu::map<std::string, AddressSet> &map, const Ticket &ticket) {
    sjtu::vector<sjtu::pair<TransferTicket, long>> vec;
    TransferTicket transfer_ticket(ticket, 0, 0);
    toTicketTree.Find(transfer_ticket, compareTo, vec);
    int size = vec.size();
    for (int i = 0; i < size; ++i) {
        try {
            map.at(std::string(vec[i].first.from)).transfer_to.push_back(
                    TicketInf2(vec[i].first.cost, vec[i].first.start_sale, vec[i].first.stop_sale,
                               vec[i].first.travel_time, vec[i].second)
            );
        } catch (...) {}//if vec1 not exist, throw
    }
}

sjtu::pair<Option, bool>
TicketSystem::CheckTransferTicket(const TicketSystem::TicketInf1 &ticket1, const TicketSystem::TicketInf2 &ticket2,
                                  const std::string &transfer) {
    //the last ticket2 set off before ticket1 arrive
    if (compareExactTime(ticket2.stop_sale, ticket1.arriving_time))return sjtu::pair<Option, bool>(Option(), false);
    Time arriving_time = ticket2.start_sale.Add(ticket1.arriving_time.TimeLag(ticket2.start_sale), ticket2.travel_time);
    Option ans(ticket1.leaving_time.IntervalMinutes(arriving_time), ticket1.cost + ticket2.cost, transfer,
               ticket1.ticket_addr, ticket2.ticket_addr);
    return sjtu::pair<Option, bool>(ans, true);
}

template<class Compare>
void TicketSystem::TryTransfer(sjtu::map<std::string, AddressSet> &map, const Compare &cmp,
                               sjtu::vector<Option> &option_vec) {
    //traverse map
    for (const auto &iter: map) {
        int size1 = iter.second.from_transfer.size();
        int size2 = iter.second.transfer_to.size();
        for (int ticket1 = 0; ticket1 < size1; ++ticket1) {
            for (int ticket2 = 0; ticket2 < size2; ++ticket2) {
                sjtu::pair<Option, bool> ans = CheckTransferTicket(iter.second.from_transfer[ticket1],
                                                                   iter.second.transfer_to[ticket2],
                                                                   iter.first);
                if (ans.second) {//ticket exists
                    option_vec.push_back(ans.first);
                }
            }
        }
    }
}

template<class Compare>
void TicketSystem::PrintOption(const sjtu::vector<Option> &option_vec, const Compare &cmp, const Time &date,
                               const std::string &from, const std::string &to,
                               FileManager<TicketDetail> &ticketFile,
                               FileManager<Seat> &seatFile) {
    if (option_vec.empty()) {
        std::cout << 0;
        return;
    }
    OptionTicket best_opt;
    int i = 0;
    for (; i < option_vec.size(); ++i) {
        OptionTicket opt(option_vec[i], ticketFile);
        if (opt.ticketDetail1.trainID == opt.ticketDetail2.trainID)continue;
        best_opt = opt;
        break;
    }
    if (i >= option_vec.size()) {
        std::cout << 0;
        return;
    }
    for (; i < option_vec.size(); ++i) {
        OptionTicket opt(option_vec[i], ticketFile);
        if (opt.ticketDetail1.trainID == opt.ticketDetail2.trainID) continue;
        if (cmp(opt, best_opt)) best_opt = opt;
    }
    //print best_opt
    int lag = date - best_opt.ticketDetail1.leaving_time;
    int address = best_opt.ticketDetail1.seat_addr +
                  (best_opt.ticketDetail1.station_num - 1) * (lag) * SEAT_SIZE;
    Time leaving = best_opt.ticketDetail1.leaving_time.Add(lag, 0);
    std::cout << best_opt.ticketDetail1.trainID << ' '
              << from << ' ' << leaving << " -> "
              << best_opt.transfer << ' ' << (leaving + best_opt.ticketDetail1.time) << ' '
              << best_opt.ticketDetail1.price << ' '
              << seatFile.MinValue(address, 0, best_opt.ticketDetail1.station_interval);
    std::cout << '\n';
    leaving = leaving + (best_opt.time - best_opt.ticketDetail2.time);
    lag = leaving.Lag(best_opt.ticketDetail2.leaving_time);
    address = best_opt.ticketDetail2.seat_addr +
              (best_opt.ticketDetail2.station_num - 1) * (lag) * SEAT_SIZE;
    std::cout << best_opt.ticketDetail2.trainID << ' '
              << best_opt.transfer << ' ' << leaving << " -> "
              << to << ' ' << (leaving + best_opt.ticketDetail2.time) << ' '
              << best_opt.ticketDetail2.price << ' '
              << seatFile.MinValue(address, 0, best_opt.ticketDetail2.station_interval);
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
    for (int start = 0; start < train.stationNum - 1; ++start) {
        for (int end = start + 1; end < train.stationNum; ++end) {
            AddTicket(station_vec[start], station_vec[end],
                      train.start_sale, train.stop_sale,
                      trainID, train.stationNum, end - start,
                      seatFile.GetAddress(train.seat_addr, start),
                      ticketFile);
        }
    }
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
    if (!time.Check()) {
        std::cout << 0;
        return;
    }
    bool flag = (keyword == "cost");//true if sort based on cost
    //find ticket from ... to ...
    sjtu::vector<sjtu::pair<Ticket, long>> vec;
    Ticket ticket(from, to, time, time);
    ticketTree.Find(ticket,
                    compareTicketStopSale, vec);
    sjtu::vector<TicketDetail> ticket_vec;
    FindTicket(ticket, vec, ticketFile, ticket_vec);
    int number = ticket_vec.size();
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
    if (!time.Check()) {
        std::cout << 0;
        return;
    }
    bool flag = (keyword == "cost");//true if sort based on cost
    //find available tickets on given day
    Ticket ticket(from, to, time, time);
    //find ticket start from the requested station (possible date)
    sjtu::map<std::string, AddressSet> map;
    FindTransferFrom(map, ticket);
    FindTransferTo(map, ticket);
    sjtu::vector<Option> option_vec;
    if (flag) {
        TryTransfer(map, costBased, option_vec);
        PrintOption(option_vec, costBased, time, from, to, ticketFile, seatFile);
    } else {
        TryTransfer(map, timeBased, option_vec);
        PrintOption(option_vec, timeBased, time, from, to, ticketFile, seatFile);
    }
}


#endif //TICKETSYSTEM_TICKET_HPP
