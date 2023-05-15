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
#include "../utility/file_manager.hpp"
#include "parameter.hpp"
#include "loginList.hpp"

class User {
    char password[30] = {'\0'};
    char name[16] = {'\0'};
    char mailAddr[30] = {'\0'};
    int privilege = 10;

public:
    User() = default;

    User(char *password_, char *name_, char *mailAddr_, int privilege_);

    friend std::ostream &operator<<(std::ostream &os, const User &information) {
        os << information.name << ' ' << information.mailAddr << ' ' << information.privilege << '\n';
        return os;
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
    BPlusTree<Username, long, CompareUsername, CompareUsername, CompareUsername> userTree{"user_tree"};

    static const char empty_str[1];

public:
    /*
     * add_user
     * return 0 if succeed
     *      new username, lower privilege
     * return -1 if failed
     * special case: add the first user(overloaded)
     */
    int AddUser(const Parameter &parameter);

    int AddUser(const Parameter &parameter, LoginList &loginList);

    /*
     * query_profile
     * print string of user_inf if succeed
     *     "username name mailAddr privilege"
     * print -1 if failed
     */
    void QueryProfile(const Parameter &parameter, const int &cur_privilege);

    /*
     * modify_profile
     * print string of modified user_inf if succeed
     *      allow default arguments
     *      lower privilege
     *     "username name mailAddr privilege"
     * print -1 if failed
     */
    void ModifyProfile(const Parameter &parameter, const int &cur_privilege);

    /*
     * login
     * check if user exist
     * add into loginList
     * (need to check if user has logged in)
     */
    int Login(const Parameter &parameter, LoginList &loginList);

    /*
     * login
     * if user isn't in loginList, find if the user exist
     * return privilege if exist
     * return false if not exist or wrong password
     */
    sjtu::pair<char *, bool> FindUser(const Parameter &parameter);

    sjtu::pair<int, bool> FindUser(const Parameter &parameter, const Username &username);

};

const char UserSystem::empty_str[1] = {'\0'};

int UserSystem::AddUser(const Parameter &parameter) {
    std::string str;
    //construct Username
    if (!parameter.GetParameter('u', str))  return -1;
    //construct User
    char password[30], name[16], mailAddr[30];
    if (!parameter.GetParameter('p', password) ||
        !parameter.GetParameter('n', name) ||
        !parameter.GetParameter('m', mailAddr))
        return -1;
//    bool flag=userTree.Insert(Username(str),User(password,name,mailAddr));
}

/*
 * SAMPLE:
 *   [3] add_user -g 10 -p aws -u I_am_the_admin -m foo@bar.com -n 奥斯卡 -c cur
 *
 */
int UserSystem::AddUser(const Parameter &parameter, LoginList &loginList) {

}


#endif //TICKETSYSTEM_USER_HPP
