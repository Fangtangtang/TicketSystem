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
#include "transaction.hpp"
#include "ticket.hpp"

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
    BPlusTree<TrainID, long, CompareTrainID, CompareTrainID, CompareTrainID> trainTree{"nodeTree_of_train",
                                                                                       "list_of_train"};

    BPlusTree<TrainID, long, CompareTrainID, CompareTrainID, CompareTrainID> releasedTree{"nodeTree_of_released",
                                                                                          "list_of_released"};

    FileManager<Train> trainInformation{"train_file"};

    FileManager<Station> stationInformation{"station_file"};

    FileManager<Seat> seatInformation{"seat_file"};

public:
    /*
     * add_train
     * return address of Station storing information of the first one
     */
    long AddStation(const Parameter &parameter);

    /*
     * add_train
     * return address of first seat(int)
     */
    long AddSeat(const Parameter &parameter);

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
    int AddTrain(const Parameter &parameter);

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
    int ReleaseTrain(const Parameter &parameter,TicketSystem &ticketSystem);

    /*
     * query_train
     * print all the information
     * return -1 if failed
     */
    void QueryTrain(const Parameter &parameter);

    void BuyTicket(const Parameter &parameter,TransactionSystem &transactionSystem);


};

#endif //TICKETSYSTEM_TRAIN_HPP
