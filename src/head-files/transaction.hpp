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

#include "../utility/tool.hpp"

struct Compare1;
struct Compare2;
const CompareUsername compare_username;

class Transaction {
    Username username;
    int timestamp = 0;
    friend Compare1;
    friend Compare2;
public:
    Transaction() = default;

    Transaction(Username username_, int timestamp_);
};

Transaction::Transaction(Username username_, int timestamp_) : username(username_), timestamp(timestamp_) {}

/*
 * used when store in file and strictFind
 */
struct Compare1 {
    bool operator()(const Transaction &a, const Transaction &b) const {
        if (compare_username.CompareStr(a.username, b.username)) {
            return compare_username(a.username, b.username);
        }
        return a.timestamp < b.timestamp;
    }
};

/*
 * used in "index_based" find
 * use username as index
 */
struct Compare2 {
    bool operator()(const Transaction &a, const Transaction &b) const {
        return compare_username(a.username, b.username);
    }
};

enum STATUS {
    success, pending, refund
};

class TransactionDetail {
    char trainID[20] = {'\0'};
    char from[30] = {'\0'};
    char to[30] = {'\0'};
    Time leaving_time;
    Time arriving_time;
    int price = 0;
    int num = 0;
    STATUS status = success;

public:
    TransactionDetail() = default;

    TransactionDetail(char *trainID_, char *from_, char *to_, const Time &leaving_time_, const Time &arriving_time_,
                      int price_, int num_, STATUS status_);

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

TransactionDetail::TransactionDetail(char *trainID_, char *from_, char *to_,
                                     const Time &leaving_time_, const Time &arriving_time_,
                                     int price_, int num_, STATUS status_) :
        leaving_time(leaving_time_),
        arriving_time(arriving_time_),
        price(price_),
        num(num_),
        status(status_) {
    memset(trainID, 0, sizeof(trainID));
    strcpy(trainID, trainID_);
    memset(from, 0, sizeof(from));
    strcpy(from, from_);
    memset(to, 0, sizeof(to));
    strcpy(to, to_);
}

#endif //TICKETSYSTEM_TRANSACTION_HPP
