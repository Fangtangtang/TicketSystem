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

/*
 * manage operations about transaction information
 * including add ,query, modify
 */
class TransactionSystem {
    BPlusIndexTree<Transaction, TransactionDetail> transactionTree{
            "transaction_tree"};

    /*
     * refund_ticket
     * increase ticket num in File
     * and rollback
     */
    static void RefundTicket(const TransactionDetail &transactionDetail,
                             const int &space, const Waiting &waiting,
                             FileManager<Seat> &seatFile, WaitingList &waitingList,
                             FileManager<WaitingOrder> &waitingListFile,
                             FileManager<TransactionDetail> &transactionFile);

public:

    /*
     * buy_ticket
     * add transaction if valid
     * return addr of transaction
     */
    long AddTransaction(const Username &username_, const int &timestamp_,
                        const TrainID &trainID_, const char *from, const char *to,
                        const Time &leaving_time_, const Time &arriving_time_,
                        const int &price_, const int &num_, const STATUS &status_, const long &seat_address_,
                        const short &start_seat, const short &end_seat,
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
                     TrainSystem &trainSystem,
                     FileManager<TransactionDetail> &transactionFile,
                     FileManager<Seat> &seatFile,
                     WaitingList &waitingList, FileManager<WaitingOrder> &waitingListFile);

};

/*
 * PRIVATE
 * ------------------------------------------------------------------------------------
 */
void TransactionSystem::RefundTicket(const TransactionDetail &transactionDetail,
                                     const int &space, const Waiting &waiting,
                                     FileManager<Seat> &seatFile, WaitingList &waitingList,
                                     FileManager<WaitingOrder> &waitingListFile,
                                     FileManager<TransactionDetail> &transactionFile) {
    Seat min_number(100000);
    Seat seat_num;
    for (int i = 0; i < space; ++i) {
        seatFile.ReadEle(transactionDetail.seat_address, i, seat_num);
        seat_num += transactionDetail.num;
        seatFile.WriteEle(transactionDetail.seat_address, i, seat_num);
        if (seat_num < min_number) min_number = seat_num;
    }
    waitingList.Rollback(waiting, min_number, waitingListFile, seatFile, transactionFile);
}

/*
 * PUBLIC
 * -------------------------------------------------------------------------------------
 */
long TransactionSystem::AddTransaction(const Username &username_, const int &timestamp_,
                                       const TrainID &trainID_, const char *from, const char *to,
                                       const Time &leaving_time_, const Time &arriving_time_,
                                       const int &price_, const int &num_,
                                       const STATUS &status_, const long &seat_address_,
                                       const short &start_seat, const short &end_seat,
                                       FileManager<TransactionDetail> &transactionFile) {
    long addr = transactionFile.GetAddress();
    transactionTree.Insert(Transaction(username_, timestamp_),
                           TransactionDetail(trainID_, from, to,
                                             leaving_time_, arriving_time_,
                                             price_, num_,
                                             status_, seat_address_, start_seat, end_seat),
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
    TransactionDetail transactionDetail;
    for (int i = size - 1; i >= 0; --i) {
        transactionFile.ReadEle(vec[i], transactionDetail);
        std::cout << '\n' << transactionDetail;
    }
}

int TransactionSystem::RefundTicket(const Parameter &parameter, LoginList &loginList, TrainSystem &trainSystem,
                                    FileManager<TransactionDetail> &transactionFile,
                                    FileManager<Seat> &seatFile,
                                    WaitingList &waitingList, FileManager<WaitingOrder> &waitingListFile) {
    //check parameter
    std::string username;
    if (!parameter.GetParameter('u', username)) return -1;
    int num = 1;
    parameter.GetParameter('n', num);
    //check login
    Username user(username);
    if (loginList.CheckLoggedIn(user) < 0)return -1;
    sjtu::vector<sjtu::pair<Transaction, long>> vec;
    transactionTree.Find(Transaction(user, 0), compareTransWeak, vec);
    if (num > vec.size()) return -1;
    TransactionDetail transactionDetail;
    transactionFile.ReadEle(vec[vec.size() - num].second, transactionDetail);
    int space = transactionDetail.end_seat - transactionDetail.start_seat;
    Waiting waiting(transactionDetail.seat_address - SEAT_SIZE * transactionDetail.start_seat,
                    vec[vec.size() - num].first.timestamp);
//    std::cout<<"\nCHECK";
//    waitingList.Check();
    if (transactionDetail.status == refunded) {
        waitingList.RemoveFromWaitingList(waiting);
        return -1;
    } else {
//        int space = transactionDetail.end_seat - transactionDetail.start_seat;
//        Waiting waiting(transactionDetail.seat_address - SEAT_SIZE * transactionDetail.start_seat,
//                        vec[vec.size() - num].first.timestamp);
//        std::cout << "!!!!!!" << parameter.GetTimestamp() << ' '
//                  << transactionDetail.seat_address - SEAT_SIZE * transactionDetail.start_seat << '\n';
        if (transactionDetail.status == success) {//success
//            std::cout<<"\nCHECK";
//            waitingList.Check();
            RefundTicket(transactionDetail, space, waiting, seatFile, waitingList, waitingListFile, transactionFile);
//            std::cout<<"\nCHECK";
//            waitingList.Check();
        }
        waitingList.RemoveFromWaitingList(waiting);
//        std::cout<<"\nCHECK";
//        waitingList.Check();
        transactionDetail.status = refunded;
//        std::cout << "\n####" << transactionDetail.trainID << ' ' << transactionDetail << '\n';
        transactionFile.WriteEle(vec[vec.size() - num].second, transactionDetail);
        return 0;
    }
}


#endif //TICKETSYSTEM_TRANSACTION_HPP
