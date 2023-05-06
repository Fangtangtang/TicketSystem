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

/*
 * unique identifier of user
 * used as key when store user_information in bpt
 *
 * ele to be stored in loginList:
 *        Key:Username
 *        Value:int (privilege)
 */
struct Compare;

class UserSystem;

class Username {
private:
    char username[20] = {'\0'};

    friend Compare;

    friend UserSystem;

public:
    Username() = default;

    Username(char *username_);
};

Username::Username(char *username_) {
    memset(username, 0, sizeof(username));
    strcpy(username, username_);
}

struct Compare {
    bool operator()(const Username &a, const Username &b) const {
        return strcmp(a.username, b.username) < 0;
    }
};

class User {
    char password[30] = {'\0'};
    char name[16] = {'\0'};
    char mailAddr[30] = {'\0'};
    int privilege = 10;

public:
    User() = default;

    User(char *password_, char *name_, char *mailAddr_, int privilege_);

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
    BPlusTree<Username, User, Compare, Compare> userInformation{"nodeTree_of_user", "list_of_user"};

public:
    
};

#endif //TICKETSYSTEM_USER_HPP
