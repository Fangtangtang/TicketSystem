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
    int price;
    int num;
    STATUS status;
};

#endif //TICKETSYSTEM_TRANSACTION_HPP
