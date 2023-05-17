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

class UserSystem;

class User {
    char password[31] = {'\0'};
    char name[16] = {'\0'};
    char mailAddr[31] = {'\0'};
    short privilege = 10;
    friend UserSystem;
public:
    User() = default;

    User(char *password_, char *name_, char *mailAddr_, short privilege_);

    friend std::ostream &operator<<(std::ostream &os, const User &information) {
        os << information.name << ' ' << information.mailAddr << ' ' << information.privilege;
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

    void ModifyPrivilege(const short privilege_) {
        privilege = privilege_;
    }
};


User::User(char *password_, char *name_, char *mailAddr_, short privilege_ = 10) :
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
    BPlusTree<Username, User> userTree{"user_tree"};

    User user;

public:
    /*
     * add_user
     * return 0 if succeed
     *      new username, lower privilege
     * return -1 if failed
     * special case: add the first user(overloaded)
     */
    int AddUser(const Parameter &parameter, FileManager<User> &userFile);

    int AddUser(const Parameter &parameter, LoginList &loginList, FileManager<User> &userFile);

    /*
     * query_profile
     * print string of user_inf if succeed
     *     "username name mailAddr privilege"
     * print -1 if failed
     */
    void QueryProfile(const Parameter &parameter, LoginList &loginList, FileManager<User> &userFile);

    /*
     * modify_profile
     * print string of modified user_inf if succeed
     *      allow default arguments
     *      lower privilege
     *     "username name mailAddr privilege"
     * print -1 if failed
     */
    void ModifyProfile(const Parameter &parameter, LoginList &loginList, FileManager<User> &userFile);

    /*
     * login
     * check if user exist
     * add into loginList
     * (need to check if user has logged in)
     */
    int Login(const Parameter &parameter, LoginList &loginList, FileManager<User> &userFile);

    /*
     * login
     * if user isn't in loginList, find if the user exist
     * return privilege if exist
     * return false if not exist or wrong password
     */
    sjtu::pair<char *, bool> FindUser(const Parameter &parameter);

    sjtu::pair<int, bool> FindUser(const Parameter &parameter, const Username &username);

};

int UserSystem::AddUser(const Parameter &parameter, FileManager<User> &userFile) {
    //check parameter
    std::string username;
    char password[31], name[16], mailAddr[31];
    if (!parameter.GetParameter('u', username) ||
        !parameter.GetParameter('p', password) ||
        !parameter.GetParameter('n', name) ||
        !parameter.GetParameter('m', mailAddr))
        return -1;
    if (userTree.Insert(Username(username), User(password, name, mailAddr), userFile, compareUsername)) return 0;
    return -1;
}

int UserSystem::AddUser(const Parameter &parameter, LoginList &loginList, FileManager<User> &userFile) {
    //check parameter
    short privilege;
    std::string username;
    char password[31], name[16], mailAddr[31];
    if (!parameter.GetParameter('g', privilege) ||
        !parameter.GetParameter('u', username) ||
        !parameter.GetParameter('p', password) ||
        !parameter.GetParameter('n', name) ||
        !parameter.GetParameter('m', mailAddr))
        return -1;
    //check privilege
    short cur_privilege = loginList.CheckLoggedIn(parameter);
    if (cur_privilege <= privilege) return -1;//permission exceeded
    if (userTree.Insert(Username(username), User(password, name, mailAddr, privilege),
                        userFile, compareUsername))
        return 0;
    return -1;
}

void UserSystem::QueryProfile(const Parameter &parameter, LoginList &loginList, FileManager<User> &userFile) {
    std::string cur_username;
    if (!parameter.GetParameter('c', cur_username)) {
        std::cout << -1;
        return;
    }
    short cur_privilege = loginList.CheckLoggedIn(Username(cur_username));
    if (cur_privilege < 0) {//cur_username not in loginList
        std::cout << -1;
        return;
    }
    //check parameter
    std::string username;
    if (!parameter.GetParameter('u', username)) {
        std::cout << -1;
        return;
    }
    sjtu::vector<long> vec;
    userTree.Find(Username(username), compareUsername, vec);
    if (vec.empty()) {//not exist
        std::cout << -1;
        return;
    }
    userFile.ReadEle(vec.front(), user);
    if (username != cur_username && user.privilege >= cur_privilege) {
        std::cout << -1;
        return;
    }
    std::cout << username << ' ' << user;
}

void UserSystem::ModifyProfile(const Parameter &parameter, LoginList &loginList, FileManager<User> &userFile) {
    std::string cur_username;
    if (!parameter.GetParameter('c', cur_username)) {
        std::cout << -1;
        return;
    }
    short cur_privilege = loginList.CheckLoggedIn(Username(cur_username));
    //get privilege
    short privilege = -1;
    parameter.GetParameter('g', privilege);
    if (cur_privilege <= privilege) {//cur_username not in loginList or permission exceeded
        std::cout << -1;
        return;
    }
    //check parameter
    std::string username;
    if (!parameter.GetParameter('u', username)) {
        std::cout << -1;
        return;
    }
    sjtu::vector<long> vec;
    userTree.Find(Username(username), compareUsername, vec);
    if (vec.empty()) {//not exist
        std::cout << -1;
        return;
    }
    long address = vec.front();
    userFile.ReadEle(address, user);
    if (username != cur_username && user.privilege >= cur_privilege) {//permission exceeded
        std::cout << -1;
        return;
    }
    //try to get parameter and modify
    if (privilege >= 0) user.privilege = privilege;
    char password[31], name[16], mailAddr[31];
    if (parameter.GetParameter('p', password)) strcpy(user.password, password);
    if (parameter.GetParameter('n', name)) strcpy(user.name, name);
    if (parameter.GetParameter('m', mailAddr)) strcpy(user.mailAddr, mailAddr);
    //rewrite in file
    userFile.WriteEle(address, user);
    std::cout << username << ' ' << user;
}

int UserSystem::Login(const Parameter &parameter, LoginList &loginList, FileManager<User> &userFile) {
    std::string username;
    char password[31];
    if (!parameter.GetParameter('u', username) ||
        !parameter.GetParameter('p', password))
        return -1;
    Username username1(username);
    if (loginList.CheckLoggedIn(username1) >= 0) return -1;//logged in
    sjtu::vector<long> vec;
    userTree.Find(Username(username), compareUsername, vec);
    if (vec.empty()) return -1;//not exist
    userFile.ReadEle(vec.front(), user);
    if (strcmp(user.password, password)) return -1;//wrong password
    loginList.Login(username1, user.privilege);
    return 0;
}


#endif //TICKETSYSTEM_USER_HPP
