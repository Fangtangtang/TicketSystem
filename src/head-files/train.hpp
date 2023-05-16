/*
 * TRAIN SYSTEM
 * manage train information
 * deal with cmd
 *         add_train
 *         delete_train
 *         release_train
 *         query_train
 *         buy_ticket
 */

#ifndef TICKETSYSTEM_TRAIN_HPP
#define TICKETSYSTEM_TRAIN_HPP


#include "../utility/bpt.hpp"
#include "../utility/pair.hpp"
#include "../utility/tool.hpp"
#include "../utility/file_manager.hpp"
#include "parameter.hpp"
#include "ticket.hpp"
#include "loginList.hpp"
#include "waitingList.hpp"

/*
 * manage operations about train information
 * including add, query, modify, delete
 */
class TrainSystem {
    /*
     * Key: TrainID
     * Value: Train
     * store basic information and some address
     */
    BPlusTree<TrainID, Train, CompareTrainID, CompareTrainID, CompareTrainID> trainTree{"train_tree"};

    friend TransactionSystem;
public:
    /*
     * add_train
     * return address of Station storing information of the first one
     */
    long AddStation(const std::string &stations);

    /*
     * add_train
     * return address of first seat(int)
     */
    long AddSeat(const std::string &seats);

    /*
     * release_train & query_train
     * return pair
     * if not found or invalid return false
     * else return Train
     */
    sjtu::pair<Train, bool> FindTrain(const TrainID &trainID_);

public:
    /*
     * add_train
     * return 0 if succeed
     * return -1 if failed
     */
    int AddTrain(const Parameter &parameter,
                 FileManager<Train> &trainFile, FileManager<Station> &stationFile, FileManager<Seat> &seatFile);

    /*
     * delete_train
     * delete unreleased train
     * return 0 if succeed
     * return -1 if failed
     */
    int DeleteTrain(const Parameter &parameter);

    /*
     * release_train
     * if not found or invalid return -1
     * else modify train.released, add into releasedTree, add tickets to ticketSystem return 0
     */
    int ReleaseTrain(const Parameter &parameter, TicketSystem &ticketSystem);

    /*
     * query_train
     * print all the information
     * return -1 if failed
     */
    void QueryTrain(const Parameter &parameter);

    void BuyTicket(const Parameter &parameter, LoginList &loginList, TransactionSystem &transactionSystem,
                   WaitingList &waitingList);


};

int TrainSystem::AddTrain(const Parameter &parameter, FileManager<Train> &trainFile, FileManager<Station> &stationFile,
                          FileManager<Seat> &seatFile) {
    //check parameter
    std::string trainID, stations, prices, stopover_times, travel_times, start_time_, sale_date_;
    int station_num, seat_num;
    char type;
    if (!parameter.GetParameter('i', trainID) ||
        !parameter.GetParameter('n', station_num) ||
        !parameter.GetParameter('m', seat_num) ||
        !parameter.GetParameter('s', stations) ||
        !parameter.GetParameter('p', prices) ||
        !parameter.GetParameter('x', start_time_) ||
        !parameter.GetParameter('t', travel_times) ||
        !parameter.GetParameter('o', stopover_times) ||
        !parameter.GetParameter('d', sale_date_) ||
        !parameter.GetParameter('y', type))
        return -1;
    //construct Time
//    if (trainTree.Insert(TrainID(trainID), Train(station_num,)))
}


#endif //TICKETSYSTEM_TRAIN_HPP
