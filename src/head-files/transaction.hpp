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
#include "waitingList.hpp"
#include "parameter.hpp"
#include "loginList.hpp"

struct CompareTransStrict;
struct CompareTransWeak;
const CompareUsername compare_username;

class Transaction {
    Username username;
    int timestamp = 0;
    friend CompareTransStrict;
    friend CompareTransWeak;
public:
    Transaction() = default;

    Transaction(Username username_, int timestamp_);

    friend bool operator<(const Transaction &a, const Transaction &b);
};

Transaction::Transaction(Username username_, int timestamp_) : username(username_), timestamp(timestamp_) {}

bool operator<(const Transaction &a, const Transaction &b) {
    int cmp = compare_username.CompareStr(a.username, b.username);
    if (cmp) {
        return cmp < 0;
    }
    return a.timestamp < b.timestamp;
}

/*
 * used when store in file and strictFind
 */
struct CompareTransStrict {
    bool operator()(const Transaction &a, const Transaction &b) const {
        int cmp = compare_username.CompareStr(a.username, b.username);
        if (cmp) {
            return cmp < 0;
        }
        return a.timestamp < b.timestamp;
    }
};

const CompareTransStrict compareTransStrict;

/*
 * used in "index_based" find
 * use username as index
 */
struct CompareTransWeak {
    bool operator()(const Transaction &a, const Transaction &b) const {
        return compare_username(a.username, b.username);
    }
};

const CompareTransWeak compareTransWeak;

class TransactionDetail {
    TrainID trainID;
    char from[31] = {'\0'};
    char to[31] = {'\0'};
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
    BPlusTree<Transaction, TransactionDetail> transactionTree{
            "transaction_tree"};

public:

    /*
     * buy_ticket
     * add transaction if valid
     * return addr of transaction
     */
    long AddTransaction(const Username &username_, const int &timestamp_,
                        const TrainID &trainID_, char *from, char *to,
                        const Time &leaving_time_, const Time &arriving_time_,
                        const int &price_, const int &num_, const STATUS &status_, const long &train_address_,
                        FileManager<TransactionDetail> &transactionFile);

    /*
     * query_order
     * print all the orders of user
     * print number of orders and information if succeed
     * print -1 if failed
     */
    void QueryOrder(const Parameter &parameter, LoginList &loginList,
                    FileManager<TransactionDetail> &transactionFile);

    /*
     * refund_ticket
     * modify status
     * return 0 and train_address in train_information file if succeed
     * return -1 if failed
     */
    int RefundTicket(const Parameter &parameter, LoginList &loginList,
                     TrainSystem &trainSystem, WaitingList &waitingList);

};

long TransactionSystem::AddTransaction(const Username &username_, const int &timestamp_,
                                       const TrainID &trainID_, char *from, char *to,
                                       const Time &leaving_time_, const Time &arriving_time_,
                                       const int &price_, const int &num_,
                                       const STATUS &status_, const long &train_address_,
                                       FileManager<TransactionDetail> &transactionFile) {
    long addr = transactionFile.GetAddress();
    transactionTree.Insert(Transaction(username_, timestamp_),
                           TransactionDetail(trainID_, from, to,
                                             leaving_time_, arriving_time_,
                                             price_, num_,
                                             status_, train_address_),
                           transactionFile, compareTransStrict);
    return addr;
}

void TransactionSystem::QueryOrder(const Parameter &parameter, LoginList &loginList,
                                   FileManager<TransactionDetail> &transactionFile) {
    std::string username;
    if (!parameter.GetParameter('u', username)) {
        std::cout << -1;
        return;
    }
    Username user(username);
    if (loginList.CheckLoggedIn(user) < 0) {
        std::cout << -1;
        return;
    }
    sjtu::vector<long> vec;
    transactionTree.Find(Transaction(user, 0), compareTransWeak, vec);
    int size = vec.size();
    std::cout << size;
    for (int i = size - 1; i >= 0; --i) {
        std::cout << '\n' << vec[i];
    }
}

#endif //TICKETSYSTEM_TRANSACTION_HPP
