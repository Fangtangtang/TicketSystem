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

class Waiting {
    long train_on_day = -1;//addr of first seat, unique for train released that day
    int timestamp = 0;

    friend CompareWaiting;
    friend CompareWaitingList;
    friend WaitingList;
public:
    Waiting() = default;

    Waiting(const long &train_on_day, const int &time) : train_on_day(train_on_day), timestamp(time) {}

    bool operator<(const Waiting &other) const;

    bool operator==(const Waiting &other) const;

    friend std::ostream &operator<<(std::ostream &os, const Waiting &information) {
        os << information.train_on_day << ' ' << information.timestamp << ';';
        return os;
    }
};

bool Waiting::operator<(const Waiting &other) const {
    if (train_on_day != other.train_on_day) return train_on_day < other.train_on_day;
    return timestamp < other.timestamp;
}

bool Waiting::operator==(const Waiting &other) const {
    if (train_on_day != other.train_on_day) return false;
    return timestamp == other.timestamp;
}

/*
 * Compare class for Waiting
 * ----------------------------------
 * CompareWaiting:
 *     used to insert
 */
struct CompareWaiting {
    bool operator()(const Waiting &a, const Waiting &b) const;
};

bool CompareWaiting::operator()(const Waiting &a, const Waiting &b) const {
    if (a.train_on_day != b.train_on_day) return a.train_on_day < b.train_on_day;
    return a.timestamp < b.timestamp;
}

const CompareWaiting compareWaiting;

struct CompareWaitingList {
    bool operator()(const Waiting &a, const Waiting &b) const;
};

bool CompareWaitingList::operator()(const Waiting &a, const Waiting &b) const {
    return a.train_on_day < b.train_on_day;
}

const CompareWaitingList compareWaitingList;

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

    friend std::ostream &operator<<(std::ostream &os, const WaitingOrder &information) {
        os << information.from << ' ' << information.to << '/';
        return os;
    }
};


/*
 * manage waiting
 * including add, find, delete
 */
class WaitingList {
    BPlusIndexTree<Waiting, WaitingOrder> waitingListTree{"waiting_list_tree"};

    void RollBack(const sjtu::pair<Waiting, long> &pair,
                  sjtu::vector<Seat> &seat_vec, Seat &min_seat, long &addr, int &num,
                  FileManager<WaitingOrder> &waitingListFile, FileManager<Seat> &seatFile,
                  FileManager<TransactionDetail> &transactionFile);

public:
    /*
     * buy_ticket
     * add into waitingList
     * key based on seat_addr and timestamp
     */
    void StartWaiting(const int &from, const int &to, const int &num,
                      const int &timestamp, const long &train_on_day,
                      const long &transaction_addr, FileManager<WaitingOrder> &waitingListFile);

    /*
     * refund_ticket
     * when refund ticket, able to get seat_addr
     * use seat_addr to find all waitingList of the seat (a vec, in timestamp order)
     * read all the seat of the train set off the same day
     * traverse the vec to rollback
     * if transaction refunded or rollback, delete from tree
     */
    void Rollback(const Waiting &waiting,
                  Seat &minimal_num, FileManager<WaitingOrder> &waitingListFile,
                  FileManager<Seat> &seatFile,
                  FileManager<TransactionDetail> &transactionFile);

    void RemoveFromWaitingList(const Waiting &waiting);

    void Check() {
        waitingListTree.Check();
    }

    void Clean();
};

/*
 * PRIVATE
 * -----------------------------------------------------------------------------------------------------------------------------------------------------------------
 */
void WaitingList::RollBack(const sjtu::pair<Waiting, long> &pair,
                           sjtu::vector<Seat> &seat_vec, Seat &min_seat, long &addr, int &num,
                           FileManager<WaitingOrder> &waitingListFile, FileManager<Seat> &seatFile,
                           FileManager<TransactionDetail> &transactionFile) {
    WaitingOrder waitingOrder;
    waitingListFile.ReadEle(pair.second, waitingOrder);
//    std::cout << "\nWAITING_INF:" << pair.first.timestamp << ' ';
//    std::cout << "\nTRY_ROLLBACK:" << waitingOrder.from << ' ' << waitingOrder.to << " " << waitingOrder.num;

    Seat seat;
    if (seat_vec.empty()) {
        addr = pair.first.train_on_day;
        for (int i = 0; i < waitingOrder.to; ++i) {
            seatFile.ReadEle(addr, seat);
            seat_vec.push_back(seat);
            addr += SEAT_SIZE;
        }
        num = waitingOrder.to;
    }
    while (num < waitingOrder.to) {
        seatFile.ReadEle(addr, seat);
        seat_vec.push_back(seat);
        addr += SEAT_SIZE;
        ++num;
    }
    seat = seat_vec[waitingOrder.to - 1];
    if (seat.num < waitingOrder.num)return;
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
//    std::cout << "\n####" << transactionDetail.trainID << ' ' << transactionDetail << '\n';
    transactionDetail.ModifyStatus(success);
//    std::cout << "\n####" << transactionDetail.trainID << ' ' << transactionDetail << '\n';
    transactionFile.WriteEle(waitingOrder.transaction_addr, transactionDetail);
    waitingListTree.Delete(pair.first);
}

/*
 * PUBLIC
 * -----------------------------------------------------------------------------------------------------------------------------------------------------------------
 */
void WaitingList::StartWaiting(const int &from, const int &to,
                               const int &num, const int &timestamp,
                               const long &train_on_day,
                               const long &transaction_addr,
                               FileManager<WaitingOrder> &waitingListFile) {
    waitingListTree.Insert(Waiting(train_on_day, timestamp),
                           WaitingOrder(from, to, num, transaction_addr), waitingListFile, compareWaiting);
}

void WaitingList::Rollback(const Waiting &waiting,
                           Seat &minimal_num,
                           FileManager<WaitingOrder> &waitingListFile,
                           FileManager<Seat> &seatFile,
                           FileManager<TransactionDetail> &transactionFile) {
    sjtu::vector<sjtu::pair<Waiting, long>> vec;
    waitingListTree.Find(waiting, compareWaitingList, vec);
    //read all the seat information into a vector
    sjtu::vector<Seat> seat_vec;
    //traverse vec
    long addr, start_addr = waiting.train_on_day;
    int num = 0;
    for (auto &i: vec) {
        RollBack(i, seat_vec, minimal_num, addr, num, waitingListFile, seatFile, transactionFile);
    }
//    std::cout << "\nVEC_SIZE:" << vec.size() << '\n';
    //modify seat file
    for (auto &i: seat_vec) {
        seatFile.WriteEle(start_addr, i);
        start_addr += SEAT_SIZE;
    }
}

void WaitingList::RemoveFromWaitingList(const Waiting &waiting) {
    waitingListTree.Delete(waiting);
}

void WaitingList::Clean() {
    waitingListTree.Clean();
}


#endif //TICKETSYSTEM_WAITINGLIST_HPP
