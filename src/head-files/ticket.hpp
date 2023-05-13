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
#include "tokenScanner.hpp"


class TimeBased;

class CostBased;

class TicketDetail {
    TrainID trainID;
    int price = 0;
    //TODO 是否保留time或arriving_time
    int time = 0;
    Time leaving_time;
    Time arriving_time;
    long seat_addr = 0;

    friend TimeBased;
    friend CostBased;
public:
    TicketDetail() = default;

    TicketDetail(const TrainID &trainID_,
                 const int &price_, const int &time_,
                 const Time &leaving_time_, const Time &arriving_time_,
                 const long &seat_addr_);

};

TicketDetail::TicketDetail(const TrainID &trainID_, const int &price_, const int &time_, const Time &leaving_time_,
                           const Time &arriving_time_, const long &seat_addr_) :
        trainID(trainID_), price(price_), time(time_),
        leaving_time(leaving_time_), arriving_time(arriving_time_),
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
    BPlusTree<Ticket, long, CompareTicket, CompareTicket2, CompareTicket2> ticketTree{"nodeTree_of_ticket",
                                                                                      "list_of_ticket"};

    FileManager<TicketDetail> ticketInformation{"ticket_file"};

    FileManager<Seat> seatInformation{"seat_file"};

public:
    /*
     * release_train
     * store ticket information of the train to file
     * TODO
     */
    void AddTicket();

    /*
     * query_ticket
     * given date from to
     * print based on time or cost
     */
    void QueryTicket(TokenScanner &tokenScanner);

    /*
     * query_transfer
     * given date from to
     * need to transfer
     * return best choice based on time or cost
     */
    void QueryTransfer(TokenScanner &tokenScanner);


};

#endif //TICKETSYSTEM_TICKET_HPP
