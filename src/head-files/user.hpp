/*
 * USER SYSTEM
 * manage user information
 * deal with cmd
 *         add_user
 *         query_profile
 *         modify_profile
 *         login
 *
 */

#ifndef TICKETSYSTEM_USER_HPP
#define TICKETSYSTEM_USER_HPP

#include "../utility/bpt.hpp"
#include "../utility/pair.hpp"
#include "../utility/tool.hpp"
#include "tokenScanner.hpp"

/*
 * unique identifier of user
 * used as key when store user_information in bpt
 *
 * ele to be stored in loginList:
 *        Key:Username
 *        Value:int (privilege)
 */

class User {
    char password[30] = {'\0'};
    char name[16] = {'\0'};
    char mailAddr[30] = {'\0'};
    int privilege = 10;

public:
    User() = default;

    User(char *password_, char *name_, char *mailAddr_, int privilege_);

    void PrintInformation() {
        std::cout << name << ' ' << mailAddr << ' ' << privilege << '\n';
    }

    void ModifyPassword(char *password_) {
        memset(password, 0, sizeof(password));
        strcpy(password, password_);
    }

    void ModifyName(char *name_) {
        memset(name, 0, sizeof(name));
        strcpy(name, name_);
    }

    void ModifyMailAddr(char *mailAddr_) {
        memset(mailAddr, 0, sizeof(mailAddr));
        strcpy(mailAddr, mailAddr_);
    }

    void ModifyPrivilege(const int privilege_) {
        privilege = privilege_;
    }
};


User::User(char *password_, char *name_, char *mailAddr_, int privilege_ = 10) :
        privilege(privilege_) {
    memset(password, 0, sizeof(password));
    strcpy(password, password_);
    memset(name, 0, sizeof(name));
    strcpy(name, name_);
    memset(mailAddr, 0, sizeof(mailAddr));
    strcpy(mailAddr, mailAddr_);
}

/*
 * manage operations about user information
 * including add, query, modify
 */
class UserSystem {
private:
    BPlusTree<Username, User, CompareUsername, CompareUsername> userInformation{"nodeTree_of_user", "list_of_user"};

    static const char empty_str[1];

public:
    /*
     * add_user
     * return 0 if succeed
     *      new username, lower privilege
     * return -1 if failed
     * special case: add the first user
     */
    int AddUser(TokenScanner &tokenScanner, const int &cur_privilege);

    /*
     * query_profile
     * print string of user_inf if succeed
     *     "username name mailAddr privilege"
     * print -1 if failed
     */
    void QueryProfile(TokenScanner &tokenScanner, const int &cur_privilege);

    /*
     * modify_profile
     * print string of modified user_inf if succeed
     *      allow default arguments
     *      lower privilege
     *     "username name mailAddr privilege"
     * print -1 if failed
     */
    void ModifyProfile(TokenScanner &tokenScanner, const int &cur_privilege);

    /*
     * login
     * if user isn't in loginList, find if the user exist
     * return privilege if exist
     * return false if not exist or wrong password
     */
    sjtu::pair<int, bool> FindUser(TokenScanner &tokenScanner, const Username &username);
};

const char UserSystem::empty_str[1] = {'\0'};

#endif //TICKETSYSTEM_USER_HPP
