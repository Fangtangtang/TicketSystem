/*
 * LOGIN
 * manage all the user login or logout
 * deal with cmd
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

class UserShortcut {
    int privilege = 10;
    long addr = -1;
public:
    UserShortcut() = default;

    UserShortcut(const int &privilege_, const long &addr_);
};

UserShortcut::UserShortcut(const int &privilege_, const long &addr_) : privilege(privilege_), addr(addr_) {}

/*
 * manage operations about loginList
 */
class LoginList {
    /*
    * Key:Username
    * Value:privilege
    */
    sjtu::map<Username, int, CompareUsername> loginList;

public:

    /*
     * login
     * return 0 and add user to list if succeed
     * return -1 if failed
     *        in list\not exist\wrong password
     */
    int Login(const Parameter &parameter, const Username &username);

    /*
     * logout
     * return 0 and erase from list if succeed
     * return -1 if failed
     */
    int Logout(const Parameter &parameter);

    /*
     * query_profile\modify_profile\buy_ticket\query_order\refund_ticket
     * if user has Logged in
     */
    bool CheckLoggedIn(const Username&username);

};

#endif //TICKETSYSTEM_LOGINLIST_HPP
