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

bool Initialize();

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
                 bool &flag);

int main() {
    //construct file_managers
    FileManager<User> userFile("user_information");
    FileManager<TransactionDetail> TransactionFile{"transaction_information"};
    FileManager<Train> trainFile("train_information");
    FileManager<Station> stationFile{"station_information"};
    FileManager<Seat> seatFile{"seat_information"};
    FileManager<WaitingTransaction> waitingListFile{"waiting_information"};
    //construct System
    UserSystem userSystem;
    TrainSystem trainSystem;
    TransactionSystem transactionSystem;
    TicketSystem ticketSystem;
    WaitingList waitingList;
    LoginList loginList;
    Parameter parameter;
    bool flag = Initialize();
    //process line in a loop
    while (std::cin) {
    ProcessLine(parameter,
                userSystem,
                trainSystem,
                transactionSystem,
                ticketSystem,
                waitingList,
                loginList,
                flag);
        if (flag)break;
    }
    return 0;
}

bool Initialize() {
    std::fstream test;
    test.open("user_file");
    if (!test.good()) {
        test.close();
        return true;
    } else {
        test.close();
        return false;
    }
}

void ProcessLine(Parameter parameter,
                 UserSystem &userSystem,
                 TrainSystem &trainSystem,
                 TransactionSystem &transactionSystem,
                 TicketSystem &ticketSystem,
                 WaitingList &waitingList,
                 LoginList &loginList,
                 bool &flag) {
    std::string cmd = parameter.ReadLine();
    /*
     * user in LoginList
     * try to add user in UserSystem
     * special case: add the first user(flag==true)
     */
    if (cmd == "add_user") {
        if (flag)std::cout << userSystem.AddUser(parameter);//add first
        else std::cout << userSystem.AddUser(parameter, loginList);
    } else if (cmd == "login") {
        std::cout << userSystem.Login(parameter, loginList);
    } else if (cmd == "logout") {
        std::cout << loginList.Logout(parameter);
    } else if (cmd == "query_profile") {
        userSystem.QueryProfile(parameter, loginList.CheckLoggedIn(parameter));
    } else if (cmd == "modify_profile") {
        userSystem.ModifyProfile(parameter, loginList.CheckLoggedIn(parameter));
    } else if (cmd == "add_train") {
        std::cout << trainSystem.AddTrain(parameter);
    } else if (cmd == "delete_train") {
        std::cout << trainSystem.DeleteTrain(parameter);
    } else if (cmd == "release_train") {
        std::cout << trainSystem.ReleaseTrain(parameter, ticketSystem);
    } else if (cmd == "query_train") {
        trainSystem.QueryTrain(parameter);
    } else if (cmd == "query_ticket") {
        ticketSystem.QueryTicket(parameter);
    } else if (cmd == "query_transfer") {
        ticketSystem.QueryTransfer(parameter);
    } else if (cmd == "buy_ticket") {
        trainSystem.BuyTicket(parameter, loginList, transactionSystem, waitingList);
    } else if (cmd == "query_order") {
        transactionSystem.QueryOrder(parameter, loginList);
    } else if (cmd == "refund_ticket") {
        transactionSystem.RefundTicket(parameter, loginList, trainSystem, waitingList);
    } else if (cmd == "clean") {
        //TODO
    } else if (cmd == "exit") {
        std::cout << "bye";
        flag = true;
    }
    std::cout << '\n';
    flag = false;
}