#include <iostream>
#include <string>
#include "utility/file_manager.hpp"
#include "head-files/loginList.hpp"
#include "head-files/parameter.hpp"
#include "head-files/ticket.hpp"
#include "head-files/transaction.hpp"
#include "head-files/user.hpp"
#include "head-files/train.hpp"
#include "head-files/waitingList.hpp"

/*
 * read request and process
 * flag == true if exit
 */
void ProcessLine(Parameter parameter,
                 UserSystem &userSystem,
                 TrainSystem &trainSystem,
                 TransactionSystem &transactionSystem,
                 TicketSystem &ticketSystem,
                 WaitingList &waitingList,
                 LoginList &loginList,
                 FileManager<User> &userFile,
                 FileManager<TransactionDetail> &TransactionFile,
                 FileManager<Train> &trainFile,
                 FileManager<Station> &stationFile,
                 FileManager<Seat> &seatFile,
                 FileManager<TicketDetail> &ticketFile,
                 FileManager<WaitingOrder> &waitingListFile,
                 bool &flag
);

int main() {
//    freopen("../my_data/User/test1", "r", stdin);
//    freopen("my_test1", "w", stdout);
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    bool flag;
    //construct file_managers
    FileManager<User> userFile("user_information");
    FileManager<TransactionDetail> transactionFile{"transaction_information"};
    FileManager<Train> trainFile("train_information");
    FileManager<Station> stationFile{"station_information"};
    FileManager<Seat> seatFile{"seat_information"};
    FileManager<WaitingOrder> waitingListFile{"waiting_information"};
    FileManager<TicketDetail> ticketFile{"ticket_information"};
    //construct System
    UserSystem userSystem;
    TrainSystem trainSystem;
    TransactionSystem transactionSystem;
    TicketSystem ticketSystem;
    WaitingList waitingList;
    LoginList loginList;
    Parameter parameter;


    //process line in a loop
    while (std::cin) {
        ProcessLine(parameter,
                    userSystem,
                    trainSystem,
                    transactionSystem,
                    ticketSystem,
                    waitingList,
                    loginList,
                    userFile,
                    transactionFile,
                    trainFile,
                    stationFile,
                    seatFile,
                    ticketFile,
                    waitingListFile,
                    flag);
        if (flag)break;
    }
    return 0;
}

void ProcessLine(Parameter parameter,
                 UserSystem &userSystem,
                 TrainSystem &trainSystem,
                 TransactionSystem &transactionSystem,
                 TicketSystem &ticketSystem,
                 WaitingList &waitingList,
                 LoginList &loginList,
                 FileManager<User> &userFile,
                 FileManager<TransactionDetail> &transactionFile,
                 FileManager<Train> &trainFile,
                 FileManager<Station> &stationFile,
                 FileManager<Seat> &seatFile,
                 FileManager<TicketDetail> &ticketFile,
                 FileManager<WaitingOrder> &waitingListFile,
                 bool &flag) {
    std::string cmd = parameter.ReadLine();
    std::cout << '[' << parameter.GetTimestamp() << "] ";
    if (cmd == "add_user") {
        std::cout << userSystem.AddUser(parameter, loginList, userFile);
    } else if (cmd == "login") {
        std::cout << userSystem.Login(parameter, loginList, userFile);
    } else if (cmd == "logout") {
        std::cout << loginList.Logout(parameter);
    } else if (cmd == "query_profile") {
        userSystem.QueryProfile(parameter, loginList, userFile);
    } else if (cmd == "modify_profile") {
        userSystem.ModifyProfile(parameter, loginList, userFile);
    } else if (cmd == "add_train") {
        std::cout << trainSystem.AddTrain(parameter, trainFile, stationFile, seatFile);
    } else if (cmd == "delete_train") {
        std::cout << trainSystem.DeleteTrain(parameter);
    } else if (cmd == "release_train") {
        std::cout << trainSystem.ReleaseTrain(parameter, ticketSystem, trainFile, stationFile, seatFile, ticketFile);
    } else if (cmd == "query_train") {
        trainSystem.QueryTrain(parameter, trainFile, stationFile, seatFile);
    } else if (cmd == "query_ticket") {
        ticketSystem.QueryTicket(parameter, ticketFile, seatFile);
    } else if (cmd == "query_transfer") {
        ticketSystem.QueryTransfer(parameter, ticketFile, seatFile);
    } else if (cmd == "buy_ticket") {
        trainSystem.BuyTicket(parameter, loginList, transactionSystem, waitingList,
                              trainFile, stationFile, seatFile, transactionFile, waitingListFile);
    } else if (cmd == "query_order") {
        transactionSystem.QueryOrder(parameter, loginList, transactionFile);
    } else if (cmd == "refund_ticket") {
        std::cout << transactionSystem.RefundTicket(parameter, loginList, trainSystem, transactionFile, seatFile,
                                                    waitingList, waitingListFile);
    } else if (cmd == "clean") {
        loginList.Clear();
        userSystem.Clean();
        trainSystem.Clean();
        waitingList.Clean();
        transactionSystem.Clean();
        ticketSystem.Clean();
    } else if (cmd == "exit") {
        std::cout << "bye";
        flag = true;
        return;
    }
    std::cout << '\n';
    flag = false;
}