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
#include "../utility/file_manager.hpp"

const CompareTrainID compareTrainID;

class Compare1;

class Compare2;

class Compare3;

/*
 * key to identify different transaction
 */
class WaitingOrder {
    TrainID trainID;
    int from = 0;//the num 'from' station
    int to = 0;
    int num = 0;
    friend Compare1;
    friend Compare2;
    friend Compare3;

public:
    WaitingOrder() = default;

    WaitingOrder(const TrainID &trainID_, const int &from_, const int &to_, const int &num_);
};

WaitingOrder::WaitingOrder(const TrainID &trainID_, const int &from_, const int &to_, const int &num_) :
        trainID(trainID_), from(from_), to(to_), num(num_) {
}

class Compare1 {
    bool operator()(const WaitingOrder &a, const WaitingOrder &b);
};

bool Compare1::operator()(const WaitingOrder &a, const WaitingOrder &b) {
    int cmp = compareTrainID.CompareStr(a.trainID, b.trainID);
    if (cmp) return cmp < 0;
    if (a.from != b.from) return a.from < b.from;
    if (a.to != b.to) return a.to < b.to;
    return a.num < b.num;
}

class Compare2 {
    bool operator()(const WaitingOrder &a, const WaitingOrder &b);
};

bool Compare2::operator()(const WaitingOrder &a, const WaitingOrder &b) {
    return a.to <= b.from;
}

class Compare3 {
    bool operator()(const WaitingOrder &a, const WaitingOrder &b);
};

bool Compare3::operator()(const WaitingOrder &a, const WaitingOrder &b) {
    return a.num <= b.num;
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

/*
 * manage waiting
 * including add, find, delete
 */                                                                                  transaction_addr(addr) {}

class WaitingList {
    BPlusTree<WaitingOrder, WaitingTransaction, Compare1, Compare2, Compare3> waitingListTree{"nodeTree_of_waiting",
                                                                                              "list_of_waiting"};
    FileManager<WaitingTransaction> waitingListInformation{"waiting_information"};

public:
    /*
     * buy_ticket
     * add into waitingList
     */
    void StartWaiting(const TrainID &trainId, const int &from, const int &to, const int &num,
                      const int &timestamp, const long &addr);

    /*
     * refund_ticket
     */
    void Rollback(const Train &train, const int &from, const int &to, const int &minimal_num);
};

#endif //TICKETSYSTEM_WAITINGLIST_HPP
