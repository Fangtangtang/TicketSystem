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
#include "tokenScanner.hpp"


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

    FileManager<Train> trainInformation{"train_file"};

    FileManager<Station> stationInformation{"station_file"};

    FileManager<Seat> seatInformation{"seat_file"};

public:
    /*
     * add_train
     * return address of Station storing information of the first one
     */
    long AddStation(TokenScanner &tokenScanner);

    /*
     * add_train
     * return address of first seat(int)
     */
    long AddSeat(TokenScanner &tokenScanner);

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
    int AddTrain(TokenScanner &tokenScanner, const TrainID &trainID_);

    /*
     * delete_train
     * delete unreleased train
     * return 0 if succeed
     * return -1 if failed
     */
    int DeleteTrain(const TrainID &trainID_);

    /*
     * release_train
     * return pair
     * if not found or invalid return false
     * else return Train
     */
    sjtu::pair<Train, bool> ReleaseTrain(const TrainID &trainID_);

    /*
     * query_train
     * print all the information
     * return -1 if failed
     */
    void QueryTrain(const TrainID &trainID_);


};

#endif //TICKETSYSTEM_TRAIN_HPP
