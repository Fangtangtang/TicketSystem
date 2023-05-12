/*
 * WAITING LIST
 * deal with cmd
 *        buy_ticket
 *        refund_ticket
 */
#ifndef TICKETSYSTEM_WAITINGLIST_HPP
#define TICKETSYSTEM_WAITINGLIST_HPP

#include "../utility/tool.hpp"
#include "../utility/bpt.hpp"

const CompareTrainID compareTrainID;

class CompareWaiting;

/*
 * key to identify different transaction
 */
class WaitingOrder {
    TrainID trainID;
    int from = 0;//the num 'from' station
    int to = 0;
    int num = 0;
    friend CompareWaiting;
public:
    WaitingOrder() = default;

    WaitingOrder(const TrainID &trainID_, const int &from_, const int &to_, const int &num_);
};

WaitingOrder::WaitingOrder(const TrainID &trainID_, const int &from_, const int &to_, const int &num_) :
        trainID(trainID_), from(from_), to(to_), num(num_) {
}

class CompareWaiting {
    bool operator()(const WaitingOrder &a, const WaitingOrder &b);

};

bool CompareWaiting::operator()(const WaitingOrder &a, const WaitingOrder &b) {
    int cmp = compareTrainID.CompareStr(a.trainID, b.trainID);
    if (cmp) return cmp < 0;
    if (a.from != b.from) return a.from < b.from;
    if (a.to != b.to) return a.to < b.to;
    return a.num < b.num;
}

/*
 * Value
 * read from file, return a vec
 * sort based on timestamp
 */
class WaitingTransaction {
    int timestamp = 0;
    long transaction_addr = -1;

public:
    WaitingTransaction() = default;

    WaitingTransaction(const int &timestamp_, const long &addr);
};

WaitingTransaction::WaitingTransaction(const int &timestamp_, const long &addr) : timestamp(timestamp_),
                                                                                  transaction_addr(addr) {}

#endif //TICKETSYSTEM_WAITINGLIST_HPP
