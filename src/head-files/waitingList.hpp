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

class CompareWaiting1;

class CompareWaiting2;

class CompareWaiting3;

/*
 * key to identify different transaction
 */
class WaitingOrder {
    TrainID trainID;
    int from = 0;//the num 'from' station
    int to = 0;
    int num = 0;
    friend CompareWaiting1;
    friend CompareWaiting2;
    friend CompareWaiting3;

    friend TrainSystem;
    friend TransactionSystem;
public:
    WaitingOrder() = default;

    WaitingOrder(const TrainID &trainID_, const int &from_, const int &to_, const int &num_);
};

WaitingOrder::WaitingOrder(const TrainID &trainID_, const int &from_, const int &to_, const int &num_) :
        trainID(trainID_), from(from_), to(to_), num(num_) {
}

class CompareWaiting1 {
    bool operator()(const WaitingOrder &a, const WaitingOrder &b);
};

bool CompareWaiting1::operator()(const WaitingOrder &a, const WaitingOrder &b) {
    int cmp = compareTrainID.CompareStr(a.trainID, b.trainID);
    if (cmp) return cmp < 0;
    if (a.from != b.from) return a.from < b.from;
    if (a.to != b.to) return a.to < b.to;
    return a.num < b.num;
}

class CompareWaiting2 {
    bool operator()(const WaitingOrder &a, const WaitingOrder &b);
};

bool CompareWaiting2::operator()(const WaitingOrder &a, const WaitingOrder &b) {
    return a.to <= b.from;
}

class CompareWaiting3 {
    bool operator()(const WaitingOrder &a, const WaitingOrder &b);
};

bool CompareWaiting3::operator()(const WaitingOrder &a, const WaitingOrder &b) {
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
    BPlusTree<WaitingOrder, WaitingTransaction, CompareWaiting1, CompareWaiting2, CompareWaiting3> waitingListTree{"waiting_list_tree"};

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
