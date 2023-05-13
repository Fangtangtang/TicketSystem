/*
 * TRANSACTION SYSTEM
 * manage transaction information
 * deal with cmd:
 *          buy_ticket
 *          query_order
 *          refund_ticket
 */

#ifndef TICKETSYSTEM_TRANSACTION_HPP
#define TICKETSYSTEM_TRANSACTION_HPP

#include "../utility/bpt.hpp"
#include "../utility/pair.hpp"
#include "../utility/tool.hpp"
#include "../utility/file_manager.hpp"
#include "tokenScanner.hpp"


struct CompareTrans1;
struct CompareTrans2;
const CompareUsername compare_username;

class Transaction {
    Username username;
    int timestamp = 0;
    friend CompareTrans1;
    friend CompareTrans2;
public:
    Transaction() = default;

    Transaction(Username username_, int timestamp_);
};

Transaction::Transaction(Username username_, int timestamp_) : username(username_), timestamp(timestamp_) {}

/*
 * used when store in file and strictFind
 */
struct CompareTrans1 {
    bool operator()(const Transaction &a, const Transaction &b) const {
        int cmp = compare_username.CompareStr(a.username, b.username);
        if (cmp) {
            return cmp < 0;
        }
        return a.timestamp < b.timestamp;
    }
};

/*
 * used in "index_based" find
 * use username as index
 */
struct CompareTrans2 {
    bool operator()(const Transaction &a, const Transaction &b) const {
        return compare_username(a.username, b.username);
    }
};

enum STATUS {
    success, pending, refund
};

class TransactionDetail {
    TrainID trainID;
    char from[30] = {'\0'};
    char to[30] = {'\0'};
    Time leaving_time;
    Time arriving_time;
    int price = 0;
    int num = 0;
    STATUS status = success;
    long train_address = 0;//store train_address in train_information file for faster read
    long next_waiting = -1;//link the waiting list

public:
    TransactionDetail() = default;

    TransactionDetail(const TrainID &trainID_, char *from_, char *to_, const Time &leaving_time_,
                      const Time &arriving_time_,
                      const int &price_, const int &num_, const STATUS &status_, const long &train_address_);

    void ModifyStatus(const STATUS &status_) {
        status = status_;
    }

    friend std::ostream &operator<<(std::ostream &os, const TransactionDetail &information) {
        //status
        if (information.status == success) os << "[success] ";
        else if (information.status == pending) os << "[pending] ";
        else os << "[refunded] ";
        //stations
        os << information.trainID << ' ';
        os << information.from << ' ' << information.leaving_time;
        os << " -> ";
        os << information.to << ' ' << information.arriving_time << ' ';
        os << information.price << ' ' << information.num << '\n';
        return os;
    }
};

TransactionDetail::TransactionDetail(const TrainID &trainID_, char *from_, char *to_,
                                     const Time &leaving_time_, const Time &arriving_time_,
                                     const int &price_, const int &num_, const STATUS &status_,
                                     const long &train_address_ = 0) :
        trainID(trainID_),
        leaving_time(leaving_time_),
        arriving_time(arriving_time_),
        price(price_),
        num(num_),
        status(status_),
        train_address(train_address_) {
    memset(from, 0, sizeof(from));
    strcpy(from, from_);
    memset(to, 0, sizeof(to));
    strcpy(to, to_);
}

/*
 * manage operations about transaction information
 * including add ,query, modify
 */
class TransactionSystem {
    BPlusTree<Transaction, TransactionDetail, CompareTrans1, CompareTrans2, CompareTrans2> TransactionTree{
            "nodeTree_of_transaction",
            "list_of_transaction"};

    FileManager<TransactionDetail> TransactionInformation{"transaction_information"};

public:

    /*
     * buy_ticket
     * add transaction if valid
     */
    void AddTransaction(const Username &username_, const int &timestamp_,
                        const TrainID &trainID_, char *from, char *to,
                        const Time &leaving_time_, const Time &arriving_time_,
                        const int &price_, const int &num_, const STATUS &status_, const long &train_address_);

    /*
     * query_order
     * print all the orders of user
     * print number of orders and information if succeed
     * print -1 if failed
     */
    void QueryOrder(const Username &username_);

    /*
     * refund_ticket
     * modify status
     * return 0 and train_address in train_information file if succeed
     * return -1 if failed
     */
    int RefundTicket(const Username &username_, const int &index_of_transaction, long &train_address_);

};

#endif //TICKETSYSTEM_TRANSACTION_HPP
