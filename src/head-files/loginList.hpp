/*
 * LOGIN
 * manage all the user login or logout
 * deal with cmd
 *          add_user
 *          login
 *          logout
 *          query_profile
 *          modify_profile
 *          buy_ticket
 *          query_order
 *          refund_ticket
 */
#ifndef TICKETSYSTEM_LOGINLIST_HPP
#define TICKETSYSTEM_LOGINLIST_HPP

#include "../utility/map.hpp"
#include "../utility/tool.hpp"
#include "parameter.hpp"

/*
 * manage operations about loginList
 */
class LoginList {
    /*
    * Key:Username
    * Value:privilege
    */
    sjtu::map<Username, short, CompareUsername> loginList;

    friend UserSystem;
    friend TrainSystem;
    friend TransactionSystem;

    /*
     * add_user\query_profile\modify_profile\buy_ticket\query_order\refund_ticket
     * if user has Logged in return privilege
     * else return -1
     */
    short CheckLoggedIn(const Parameter &parameter);

    /*
     * buy_ticket\query_order\refund_ticket
     * if user has Logged in return privilege
     * else return -1
     */
    short CheckLoggedIn(const Username &username);

public:

    /*
     * login
     * add user to list
     */

    void Login(const Username &username, const short &privilege);

    /*
     * logout
     * return 0 and erase from list if succeed
     * return -1 if failed
     */
    int Logout(const Parameter &parameter);

    void Clear();
};

short LoginList::CheckLoggedIn(const Parameter &parameter) {
    std::string username;
    if (!parameter.GetParameter('c', username)) return -1;//missing parameter
    sjtu::pair<sjtu::map<Username, short, CompareUsername>::iterator, bool> iter = loginList.Find(Username(username));
    if (iter.second) return iter.first->second;
    return -1;
}

short LoginList::CheckLoggedIn(const Username &username) {
    sjtu::pair<sjtu::map<Username, short, CompareUsername>::iterator, bool> iter = loginList.Find(username);
    if (iter.second) return iter.first->second;
    return -1;
}

void LoginList::Login(const Username &username, const short &privilege) {
    loginList.insert(sjtu::pair<Username, short>(username, privilege));
}

int LoginList::Logout(const Parameter &parameter) {
    std::string username;
    if (!parameter.GetParameter('u', username)) return -1;
    sjtu::map<Username, short, CompareUsername>::iterator iter = loginList.find(Username(username));
    if (iter == loginList.end()) return -1;
    loginList.erase(iter);
    return 0;
}

void LoginList::Clear() {
    loginList.clear();
}

#endif //TICKETSYSTEM_LOGINLIST_HPP
