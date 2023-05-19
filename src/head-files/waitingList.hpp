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

/*
 * Key of waiting
 * ----------------------------
 * waiting list for seat, use seat_addr as key to identify
 * in timestamp order
 */
class CompareWaiting;

class CompareWaitingList;

class IsToBeModified;

class Waiting {
    long start_seat_addr = -1;
    long end_seat_addr = -1;
    int timestamp = 0;

    friend CompareWaiting;
    friend CompareWaitingList;
    friend IsToBeModified;
public:
    Waiting() = default;

    Waiting(const long &start_addr, const long &end_addr, const int &time) : start_seat_addr(start_addr),
                                                                             end_seat_addr(end_addr), timestamp(time) {}

    bool operator<(const Waiting &other) const;
};

bool Waiting::operator<(const Waiting &other) const {
    if (end_seat_addr != other.end_seat_addr) return end_seat_addr < other.end_seat_addr;
    if (start_seat_addr != other.start_seat_addr) return start_seat_addr < other.start_seat_addr;
    return timestamp < other.timestamp;
}

/*
 * Compare class for Waiting
 * ----------------------------------
 * CompareWaiting:
 *     used to insert
 *
 * CompareWaitingList:
 *     used to find
 */
struct CompareWaiting {
    bool operator()(const Waiting &a, const Waiting &b) const;
};

bool CompareWaiting::operator()(const Waiting &a, const Waiting &b) const {
    if (a.end_seat_addr != b.end_seat_addr) return a.end_seat_addr < b.end_seat_addr;
    if (a.start_seat_addr != b.start_seat_addr) return a.start_seat_addr < b.start_seat_addr;
    return a.timestamp < b.timestamp;
}

const CompareWaiting compareWaiting;

struct CompareWaitingList {
    bool operator()(const Waiting &a, const Waiting &b) const;
};

bool CompareWaitingList::operator()(const Waiting &a, const Waiting &b) const {
    return a.end_seat_addr < b.end_seat_addr;
}

const CompareWaitingList compareWaitingList;

struct IsToBeModified {
    bool operator()(const Waiting &a, const Waiting &b) const;
};

//a:target ==
bool IsToBeModified::operator()(const Waiting &a, const Waiting &b) const {
    if (a.end_seat_addr <= b.end_seat_addr && a.start_seat_addr >= b.start_seat_addr)return true;
    return false;
}

/*
 * WaitingOrder class
 * vale of bpt
 * if rollback, change status in transaction
 */
class WaitingOrder {
    int from = 0;//the index 'from' station
    int to = 0;
    int num = 0;//num of ticket
    long transaction_addr = -1;

    friend TrainSystem;
    friend TransactionSystem;
public:
    WaitingOrder() = default;

    WaitingOrder(const int &from_, const int &to_,
                 const int &num_, const long &addr) : from(from_), to(to_), num(num_), transaction_addr(addr) {}
};


/*
 * manage waiting
 * including add, find, delete
 */
class WaitingList {
    BPlusIndexTree<Waiting, WaitingOrder> waitingListTree{"waiting_list_tree"};

public:
    /*
     * buy_ticket
     * add into waitingList
     * key based on seat_addr and timestamp
     */
    void StartWaiting(const TrainID &trainId, const int &from, const int &to, const int &num,
                      const int &timestamp, const long &start_seat_addr, const long &end_seat_addr,
                      const long &transaction_addr, FileManager<WaitingOrder> &waitingListFile);

    /*
     * refund_ticket
     * when refund ticket, able to get seat_addr
     * use seat_addr to find all waitingList of the seat (a vec, in timestamp order)
     * read all the seat of the train set off the same day
     * traverse the vec to rollback
     * if transaction refunded or rollback, delete from tree
     */
    void Rollback(const long &start_seat_addr, const long &end_seat_addr, const Seat &minimal_num);
};

void WaitingList::StartWaiting(const TrainID &trainId, const int &from, const int &to,
                               const int &num, const int &timestamp,
                               const long &start_seat_addr, const long &end_seat_addr,
                               const long &transaction_addr,
                               FileManager<WaitingOrder> &waitingListFile) {
    waitingListTree.Insert(Waiting(start_seat_addr, end_seat_addr, timestamp),
                           WaitingOrder(from, to, num, transaction_addr), waitingListFile, compareWaiting);
}

#endif //TICKETSYSTEM_WAITINGLIST_HPP
