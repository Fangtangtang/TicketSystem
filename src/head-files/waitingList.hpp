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

class CompareTimestamp;

class IsToBeModified;

class Waiting {
    long start_seat_addr = -1;
    long end_seat_addr = -1;
    int timestamp = 0;

    friend CompareWaiting;
    friend CompareWaitingList;
    friend CompareTimestamp;
    friend IsToBeModified;
    friend WaitingList;
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

struct CompareTimestamp {
    bool operator()(const Waiting &a, const Waiting &b) const;

    bool operator()(const sjtu::pair<Waiting, long> &a, const sjtu::pair<Waiting, long> &b) const;

};

bool CompareTimestamp::operator()(const Waiting &a, const Waiting &b) const {
    return a.timestamp < b.timestamp;
}

bool CompareTimestamp::operator()(const sjtu::pair<Waiting, long> &a, const sjtu::pair<Waiting, long> &b) const {
    return a.first.timestamp < b.first.timestamp;
}

const CompareTimestamp compareTimestamp;

struct IsToBeModified {
    bool operator()(const Waiting &a, const Waiting &b) const;
};

//a:target ==
bool IsToBeModified::operator()(const Waiting &a, const Waiting &b) const {
    if (a.end_seat_addr <= b.end_seat_addr && a.start_seat_addr >= b.start_seat_addr)return true;
    return false;
}

const IsToBeModified isToBeModified;

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
    friend WaitingList;
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

    static void RollBack(const sjtu::pair<Waiting, long> &pair,
                         sjtu::vector<Seat> &seat_vec, Seat &min_seat, long &addr,
                         long &start_addr,
                         FileManager<WaitingOrder> &waitingListFile, FileManager<Seat> &seatFile,
                         FileManager<TransactionDetail> &transactionFile);

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
    void Rollback(const long &start_seat_addr, const long &end_seat_addr,
                  Seat &minimal_num, FileManager<WaitingOrder> &waitingListFile,
                  FileManager<Seat> &seatFile,
                  FileManager<TransactionDetail> &transactionFile);
};

/*
 * PRIVATE
 * -----------------------------------------------------------------------------------------------------------------------------------------------------------------
 */
void WaitingList::RollBack(const sjtu::pair<Waiting, long> &pair,
                           sjtu::vector<Seat> &seat_vec, Seat &min_seat, long &addr,
                           long &start_addr,
                           FileManager<WaitingOrder> &waitingListFile, FileManager<Seat> &seatFile,
                           FileManager<TransactionDetail> &transactionFile) {
    WaitingOrder waitingOrder;
    waitingListFile.ReadEle(pair.second, waitingOrder);
    if (min_seat.num < waitingOrder.num)return;//exceed
    Seat seat;
    if (seat_vec.empty()) {
        addr = seatFile.GetAddress(pair.first.start_seat_addr, -waitingOrder.from);
        start_addr = addr;
        for (int i = 0; i < waitingOrder.to; ++i) {
            seatFile.ReadEle(addr, seat);
            seat_vec.push_back(seat);
            addr += SEAT_SIZE;
        }
    }
    while (addr <= pair.first.end_seat_addr) {
        seatFile.ReadEle(addr, seat);
        seat_vec.push_back(seat);
        addr += SEAT_SIZE;
    }
    seat = seat_vec[waitingOrder.to - 1];
    for (int i = waitingOrder.from; i < waitingOrder.to - 1; ++i) {
        seat = seat_vec[i] < seat ? seat_vec[i] : seat;
        if (seat.num < waitingOrder.num)return;
    }
    //succeed, modify seat and transaction status
    for (int i = waitingOrder.from; i < waitingOrder.to; ++i) {
        seat_vec[i] -= waitingOrder.num;
    }
    TransactionDetail transactionDetail;
    transactionFile.ReadEle(waitingOrder.transaction_addr, transactionDetail);
    transactionDetail.ModifyStatus(success);
    transactionFile.WriteEle(waitingOrder.transaction_addr, transactionDetail);
    min_seat -= waitingOrder.num;
}

/*
 * PUBLIC
 * -----------------------------------------------------------------------------------------------------------------------------------------------------------------
 */
void WaitingList::StartWaiting(const TrainID &trainId, const int &from, const int &to,
                               const int &num, const int &timestamp,
                               const long &start_seat_addr, const long &end_seat_addr,
                               const long &transaction_addr,
                               FileManager<WaitingOrder> &waitingListFile) {
    waitingListTree.Insert(Waiting(start_seat_addr, end_seat_addr, timestamp),
                           WaitingOrder(from, to, num, transaction_addr), waitingListFile, compareWaiting);
}

void WaitingList::Rollback(const long &start_seat_addr, const long &end_seat_addr,
                           Seat &minimal_num,
                           FileManager<WaitingOrder> &waitingListFile,
                           FileManager<Seat> &seatFile,
                           FileManager<TransactionDetail> &transactionFile) {
    sjtu::vector<sjtu::pair<Waiting, long>> vec;
    waitingListTree.Find(Waiting(start_seat_addr, end_seat_addr, 0), compareWaitingList, isToBeModified, vec);
    sjtu::Sort(vec, 0, vec.size() - 1, compareTimestamp);//sort based on timestamp
    //read all the seat information into a vector
    sjtu::vector<Seat> seat_vec;
    //traverse vec
    long addr, start_addr;
    for (auto &i: vec) {
        RollBack(i, seat_vec, minimal_num, addr, start_addr, waitingListFile, seatFile, transactionFile);
        if (minimal_num.num == 0) break;
    }
    //modify seat file
    for (auto &i: seat_vec) {
        seatFile.WriteEle(start_addr, i);
        start_addr += SEAT_SIZE;
    }
}


#endif //TICKETSYSTEM_WAITINGLIST_HPP
