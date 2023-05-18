/*
 * toolClasses
 */
#ifndef TICKETSYSTEM_TOOL_HPP
#define TICKETSYSTEM_TOOL_HPP

#include <cstring>
#include <iostream>
#include <utility>

enum STATUS {
    success, pending, refund
};

class TrainSystem;

class TicketSystem;

class UserSystem;

class TransactionSystem;

/*
 * Username class
 * ----------------------------------------------------------------------------------------------------
 * used in UserSystem, TransactionSystem, LoginList
 * basic Compare class included
 *
 */
struct CompareUsername;

class Username {
    char username[21] = {'\0'};
    friend CompareUsername;
public:
    Username() = default;

    explicit Username(char *username_);

    explicit Username(const std::string &username_);

    bool operator<(const Username &other) const;

    bool operator==(const Username &other) const;
};

Username::Username(char *username_) {
    memset(username, 0, sizeof(username));
    strcpy(username, username_);
}

Username::Username(const std::string &username_) {
    memset(username, 0, sizeof(username));
    strcpy(username, username_.c_str());
}

bool Username::operator<(const Username &other) const {
    return strcmp(username, other.username) < 0;
}

bool Username::operator==(const Username &other) const {
    return strcmp(username, other.username) == 0;
}

/*
 * Compare class for Username
 * =============================================================================
 */
struct CompareUsername {
    bool operator()(const Username &a, const Username &b) const {
        return strcmp(a.username, b.username) < 0;
    }

    int CompareStr(const Username &a, const Username &b) const {
        return strcmp(a.username, b.username);
    }
};

const CompareUsername compareUsername;

/*
 * TrainID class
 * ----------------------------------------------------------------------------------------------------
 * used in TrainSystem, TransactionSystem, ReleasedTrainSystem
 * basic Compare class included
 */
struct CompareTrainID;

class TrainID {
    char trainID[21] = {'\0'};
    friend CompareTrainID;
public:
    TrainID() = default;

    explicit TrainID(char *trainID_);

    explicit TrainID(const std::string &trainID_);

    friend bool operator<(const TrainID &a, const TrainID &b);

    friend bool operator==(const TrainID &a, const TrainID &b);

    friend std::ostream &operator<<(std::ostream &os, const TrainID &information);

};

TrainID::TrainID(char *trainID_) {
    memset(trainID, 0, sizeof(trainID));
    strcpy(trainID, trainID_);
}

TrainID::TrainID(const std::string &trainID_) {
    memset(trainID, 0, sizeof(trainID));
    strcpy(trainID, trainID_.c_str());
}

bool operator<(const TrainID &a, const TrainID &b) {
    return strcmp(a.trainID, b.trainID) < 0;
}

bool operator==(const TrainID &a, const TrainID &b) {
    return strcmp(a.trainID, b.trainID) == 0;
}

std::ostream &operator<<(std::ostream &os, const TrainID &information) {
    os << information.trainID;
    return os;
}


/*
 * Compare class for TrainID
 * =============================================================================
 */
struct CompareTrainID {
    bool operator()(const TrainID &a, const TrainID &b) {
        return strcmp(a.trainID, b.trainID) < 0;
    }

    int CompareStr(const TrainID &a, const TrainID &b) const {
        return strcmp(a.trainID, b.trainID);
    }
};

const CompareTrainID compareTrainID;

/*
 * Interval class
 * -------------------------------------------------------------------------------------------------------------
 * manage relevant time
 */
class Time;

class Interval {
    int minutes = 0;
    friend Time;
public:
    Interval() = default;

    explicit Interval(const int &a) : minutes(a) {}

    explicit Interval(const std::string &str) {
        int para1 = (str[0] - '0') * 10 + str[1] - '0';
        int para2 = (str[3] - '0') * 10 + str[4] - '0';
        minutes = para1 * 60 + para2;
    }

    //return added
    //this added
    Interval operator+(const int &delta) {
        minutes += delta;
        return *this;
    }

    Interval operator+=(const int &delta) {
        minutes += delta;
        return *this;
    }

    int operator-(const Interval &other) const {
        return minutes - other.minutes;
    }
};

/*
 * Time class
 * -------------------------------------------------------------------------------------------------------------
 * manage date and time from June 1st to August 31st
 */
class Time {
    int minutes = 0;

public:
    Time() = default;

    explicit Time(const int &a) : minutes(a) {}

    explicit Time(const std::string &str) {
        int para1 = (str[0] - '0') * 10 + str[1] - '0';
        int para2 = (str[3] - '0') * 10 + str[4] - '0';
        if (para1 == 8) para2 += 61;
        else if (para1 == 7) para2 += 30;
        --para2;
        minutes = para2 * 24 * 60;
    }

    Time(int hour, int minute, int month = 6, int day = 1) {
        if (month == 8)day += 61;
        if (month == 7)day += 30;
        --day;
        hour += day * 24;
        minutes = minute + hour * 60;
    }

    Time operator+(const Interval &interval) const {
        return Time(minutes + interval.minutes);
    }

    Time operator+(const int &interval) const {
        return Time(minutes + interval);
    }

    friend bool operator<(const Time &a, const Time &b) {
        return (a.minutes / 1440) < (b.minutes / 1440);
    }

    friend bool operator==(const Time &a, const Time &b) {
        return (a.minutes / 1440) == (b.minutes / 1440);
    }

    friend int operator-(const Time &a, const Time &b) {
        return 1 + (a.minutes - b.minutes) / 1440;
    }

    friend std::ostream &operator<<(std::ostream &os, const Time &time_) {
        int month, day, hour, minute = time_.minutes;
        //convert
        hour = minute / 60;
        minute %= 60;
        day = hour / 24;
        hour %= 24;
        ++day;
        if (day > 61) {
            month = 8;
            day -= 61;
        } else {
            if (day > 30) {
                month = 7;
                day -= 30;
            } else month = 6;
        }
        //print
        os << 0 << month << '-';
        if (day < 9)os << 0;
        os << day << ' ';
        if (hour < 9)os << 0;
        os << hour << ':';
        if (minute < 9)os << 0;
        os << minute;
        return os;
    }

    void PrintInformation(const int &interval) const {
        int month, day, hour, minute = minutes;
        //convert
        hour = minute / 60;
        minute %= 60;
        day = hour / 24;
        hour %= 24;
        ++day;
        day += interval;
        if (day > 61) {
            month = 8;
            day -= 61;
        } else {
            if (day > 30) {
                month = 7;
                day -= 30;
            } else month = 6;
        }
        //print
        std::cout << 0 << month << '-';
        if (day < 9)std::cout << 0;
        std::cout << day << ' ';
        if (hour < 9)std::cout << 0;
        std::cout << hour << ':';
        if (minute < 9)std::cout << 0;
        std::cout << minute;
    }

    int Lag(const Time &other) const {
        return minutes > other.minutes ? (minutes / 1440 - other.minutes / 1440)
                                       : (other.minutes / 1440 - minutes / 1440);
    }

    //time in day
    Interval CalTime() const {
        return Interval(minutes % 1440);
    }

    void AddDay(const int &day) {
        minutes += day * 1440;
    }
};


class MyTime {
    short month = 0;
    short date = 0;
    short hour = 0;
    short minute = 0;
public:
    MyTime() = default;

    MyTime(short month_, short date_, short hour_, short minute_) : month(month_), date(date_),
                                                                    hour(hour_), minute(minute_) {};

    friend std::ostream &operator<<(std::ostream &os, const MyTime &time_) {
        os << 0 << time_.month << '-';
        if (time_.date < 9)os << 0;
        os << time_.date << ' ';
        if (time_.hour < 9)os << 0;
        os << time_.hour << ':';
        if (time_.minute < 9)os << 0;
        os << time_.minute;
        return os;
    }

    friend MyTime operator+(const MyTime &a, const MyTime &b) {
        auto minute_ = short(a.minute + b.minute);
        auto hour_ = short(a.hour + b.hour + minute_ / 60);
        minute_ %= 60;
        auto date_ = short(a.date + b.date + hour_ / 24);
        hour_ %= 24;
        auto month_ = short(a.month + b.month);
        if (month_ == 6) {
            if (date_ > 30) {
                ++month_;
                date_ -= 30;
            }
        }
        if (month_ == 7) {
            if (date_ > 31) {
                ++month_;
                date_ -= 31;
            }
        }
        return {month_, date_, hour_, minute_};
    }

    /*
     * add minute_ to time
     * calculate from relative time to accurate time
     */
    void addMinute(const short &minute_) {
        minute = short(minute_ + minute);
        hour = short(hour + minute / 60);
        minute %= 60;
        date = short(date + hour / 24);
        hour %= 24;
        if (month == 6) {
            if (date > 30) {
                ++month;
                date -= 30;
            }
        }
        if (month == 7) {
            if (date > 31) {
                ++month;
                date -= 31;
            }
        }
    }
};

/*
 * Train class
 * -------------------------------------------------------------------------------------------------------------
 * value type stored in train_file
 *       basic information about train,
 *       address of station_name_file,
 *       address of seat_number_file,
 */
class Train {
    int stationNum = 0;
    Time start_sale;
    Time stop_sale;
    long station_addr = 0;
    long seat_addr = 0;
    char type = '\0';
    bool released = false;
    friend TrainSystem;
    friend TicketSystem;
public:
    Train() = default;

    Train(const int &stationNum_, const Time &start_sale_, const Time &stop_sale_, const char &type_,
          const long &station, const long &seat);

    bool Release();
};

Train::Train(const int &stationNum_, const Time &start_sale_, const Time &stop_sale_, const char &type_,
             const long &station, const long &seat) :
        stationNum(stationNum_), start_sale(start_sale_), stop_sale(stop_sale_), type(type_),
        station_addr(station), seat_addr(seat) {}

bool Train::Release() {
    if (released)return false;
    released = true;
    return true;
}

/*
 * Station class
 * -------------------------------------------------------------------------------------------------------------
 * name of the station, 
 * price of ticket from the current station to next station,
 * arriving time and leaving time
 */

class Station {
    char name[31] = {'\0'};
    int price = 0;
    Interval arriving_time;
    Interval leaving_time;
    friend TrainSystem;
    friend TicketSystem;
public:
    Station() = default;

    Station(char *name_, const int &price, const Interval &arriving_time_, const Interval &leaving_time);

    Station(const std::string &name_, const int &price, const Interval &arriving_time_,
            const Interval &leaving_time);
};

Station::Station(char *name_, const int &price_, const Interval &arriving_time_,
                 const Interval &leaving_time) :
        price(price_), arriving_time(arriving_time_), leaving_time(leaving_time) {
    memset(name, 0, sizeof(name));
    strcpy(name, name_);
}

Station::Station(const std::string &name_, const int &price_, const Interval &arriving_time_,
                 const Interval &leaving_time) :
        price(price_), arriving_time(arriving_time_), leaving_time(leaving_time) {
    memset(name, 0, sizeof(name));
    strcpy(name, name_.c_str());
}


/*
 * Seat class
 * -------------------------------------------------------------------------------------------------------------
 * number of available seat
 * address of first transaction on waiting list
 */
class Seat {
    int num = 0;
    long waiting = -1;
    friend TrainSystem;
public:
    Seat() = default;

    explicit Seat(const int &seat_num) : num(seat_num) {}

    bool operator<(const Seat &a) const;

    friend std::ostream &operator<<(std::ostream &os, const Seat &information);

};

bool Seat::operator<(const Seat &a) const {
    return num < a.num;
}

std::ostream &operator<<(std::ostream &os, const Seat &information) {
    os << information.num;
    return os;
}

/*
 * Ticket class
 * -------------------------------------------------------------------------------------------------------------
 * manage basic information of ticket between two stations
 * store
 */

class CompareTicket;

class CompareFrom;

class CompareTo;

class CompareTicketStopSale;

class CompareFromTicket;

class CompareToTicket;

class IsAvailableTicket;

class IsAvailableFrom;

class IsAvailableTo;

class Ticket {
    char from[31] = {'\0'};
    char to[31] = {'\0'};
    Time start_sale;//leaving date
    Time stop_sale;

    friend CompareTicket;
    friend CompareFrom;
    friend CompareTo;
    friend CompareTicketStopSale;
    friend CompareFromTicket;
    friend CompareToTicket;
    friend IsAvailableTicket;
    friend IsAvailableFrom;
    friend IsAvailableTo;
public:
    Ticket() = default;

    Ticket(const char *from_, const char *to_, const Time &start_sale_, const Time &stop_sale_);

    Ticket(const std::string &from_, const std::string &to_);

    Ticket(const std::string &from_, const std::string &to_, const Time &start_sale_, const Time &stop_sale_);

    bool operator<(const Ticket &a) const;
};


Ticket::Ticket(const char *from_, const char *to_, const Time &start_sale_, const Time &stop_sale_) :
        start_sale(start_sale_), stop_sale(stop_sale_) {
    memset(from, 0, sizeof(from));
    strcpy(from, from_);
    memset(to, 0, sizeof(to));
    strcpy(to, to_);
}

Ticket::Ticket(const std::string &from_, const std::string &to_) {
    memset(from, 0, sizeof(from));
    strcpy(from, from_.c_str());
    memset(to, 0, sizeof(to));
    strcpy(to, to_.c_str());
}

Ticket::Ticket(const std::string &from_, const std::string &to_, const Time &start_sale_, const Time &stop_sale_) :
        start_sale(start_sale_), stop_sale(stop_sale_) {
    memset(from, 0, sizeof(from));
    strcpy(from, from_.c_str());
    memset(to, 0, sizeof(to));
    strcpy(to, to_.c_str());
}

bool Ticket::operator<(const Ticket &a) const {
    int cmp = strcmp(from, a.from);
    if (cmp) return cmp < 0;
    cmp = strcmp(to, a.to);
    if (cmp) return cmp < 0;
    if (!(stop_sale == a.stop_sale)) return stop_sale < a.stop_sale;
    return start_sale < a.start_sale;

}

/*
 * Compare class for Ticket
 * ===============================================================================
 * CompareTicket:
 *     used to insert in index tree
 *     from to stop_sale start_sale
 *
 * CompareFrom:
 *     used to insert in real from bpt
 *     ---------------------
 *     this tree is used to find where can go 
 *     at the given day from the given place 
 *     ---------------------
 *     from stop_sale start_sale to
 *
 * CompareTo:
 *     used to insert in real to bpt
 *     ---------------------
 *     this tree is used to find where can from 
 *     at the given day to the given place 
 *     ---------------------
 *     to stop_sale start_sale from
 *     
 * CompareTicketStopSale:
 *     from to stop_sale(cmp1 of query_ticket)
 *
 * CompareFromTicket:
 *     from stop_sale(cmp1 of fromTicketTree)
 *     
 * CompareToTicket:
 *     to stop_sale(cmp1 of toTicketTree)
 *     
 * IsAvailableTicket:
 *     == (cmp2 of query_ticket)
 *     
 * IsAvailableFrom:
 *     == (cmp2 of fromTicketTree)
 * 
 * IsAvailableTo:
 *     == (cmp2 of to/ticketTree)
 *     
 */
struct CompareTicket {
    bool operator()(const Ticket &a, const Ticket &b) const;
};

bool CompareTicket::operator()(const Ticket &a, const Ticket &b) const {
    int cmp = strcmp(a.from, b.from);
    if (cmp) return cmp < 0;
    cmp = strcmp(a.to, b.to);
    if (cmp) return cmp < 0;
    if (!(a.stop_sale == b.stop_sale)) return a.stop_sale < b.stop_sale;
    return a.start_sale < b.start_sale;
}

const CompareTicket compareTicket;

struct CompareFrom {
    bool operator()(const Ticket &a, const Ticket &b) const;
};

bool CompareFrom::operator()(const Ticket &a, const Ticket &b) const {
    int cmp = strcmp(a.from, b.from);
    if (cmp) return cmp < 0;
    if (!(a.stop_sale == b.stop_sale)) return a.stop_sale < b.stop_sale;
    if (!(a.start_sale == b.start_sale))return a.start_sale < b.start_sale;
    return strcmp(a.to, b.to);
}

const CompareFrom compareFrom;

struct CompareTo {
    bool operator()(const Ticket &a, const Ticket &b) const;
};

bool CompareTo::operator()(const Ticket &a, const Ticket &b) const {
    int cmp = strcmp(a.to, b.to);
    if (cmp) return cmp < 0;
    if (!(a.stop_sale == b.stop_sale)) return a.stop_sale < b.stop_sale;
    if (!(a.start_sale == b.start_sale))return a.start_sale < b.start_sale;
    return strcmp(a.from, b.from);
}

const CompareTo compareTo;

struct CompareTicketStopSale {
    bool operator()(const Ticket &a, const Ticket &b) const;
};

bool CompareTicketStopSale::operator()(const Ticket &a, const Ticket &b) const {
    int cmp = strcmp(a.from, b.from);
    if (cmp) return cmp < 0;
    cmp = strcmp(a.to, b.to);
    if (cmp) return cmp < 0;
    return a.stop_sale < b.stop_sale;
}

const CompareTicketStopSale compareTicketStopSale;

struct CompareFromTicket {
    bool operator()(const Ticket &a, const Ticket &b) const;
};

bool CompareFromTicket::operator()(const Ticket &a, const Ticket &b) const {
    int cmp = strcmp(a.from, b.from);
    if (cmp) return cmp < 0;
    return a.stop_sale < b.stop_sale;
}

const CompareFromTicket compareFromTicket;


struct CompareToTicket {
    bool operator()(const Ticket &a, const Ticket &b) const;
};

bool CompareToTicket::operator()(const Ticket &a, const Ticket &b) const {
    int cmp = strcmp(a.to, b.to);
    if (cmp) return cmp < 0;
    return a.stop_sale < b.stop_sale;
}

const CompareToTicket compareToTicket;

struct IsAvailableTicket {
    bool operator()(const Ticket &a, const Ticket &b) const;
};

//a:target
bool IsAvailableTicket::operator()(const Ticket &a, const Ticket &b) const {
    if (strcmp(a.from, b.from)) return false;
    if (strcmp(a.to, b.to)) return false;
    if (a.stop_sale < b.start_sale) return false;
    return true;
}

const IsAvailableTicket isAvailableTicket;

struct IsAvailableFrom {
    bool operator()(const Ticket &a, const Ticket &b) const;
};

//a:target
bool IsAvailableFrom::operator()(const Ticket &a, const Ticket &b) const {
    if (strcmp(a.from, b.from)) return false;
    if (a.stop_sale < b.start_sale) return false;
    return true;
}

const IsAvailableFrom isAvailableFrom;

struct IsAvailableTo {
    bool operator()(const Ticket &a, const Ticket &b) const;
};

//a:target
bool IsAvailableTo::operator()(const Ticket &a, const Ticket &b) const {
    if (strcmp(a.to, b.to)) return false;
    if (a.stop_sale < b.start_sale) return false;
    return true;
}

const IsAvailableTo isAvailableTo;
/*
 * class Scanner
 * ======================================================================================
 * deal with a|b|c
 */
template<class Type>
class Scanner;

template<>
class Scanner<std::string> {
    std::string input;
    int length = 0;
    int tokenStart = 0, tokenEnd = 0;

    void Update() {
        ++tokenEnd;
        tokenStart = tokenEnd;
    }

public:
    explicit Scanner(std::string str) : input(std::move(str)) {
        length = input.length();
    }

    void GetStr(std::string &string) {
        while (input[tokenEnd] != '|') {
            ++tokenEnd;
            if (tokenEnd == length) break;
        }
        string = input.substr(tokenStart, tokenEnd - tokenStart);
        Update();
    }

};


template<>
class Scanner<int> {
    std::string input;
    int length = 0;
    int tokenStart = 0, tokenEnd = 0;

    void Update() {
        ++tokenEnd;
        tokenStart = tokenEnd;
    }

public:
    explicit Scanner(std::string str) : input(std::move(str)) {
        length = input.length();
    }

    void GetNum(int &intNum) {
        intNum = 0;
        while (input[tokenEnd] != '|') {
            intNum *= 10;
            intNum += (input[tokenEnd] - '0');
            ++tokenEnd;
            if (tokenEnd == length) break;
        }
        Update();
    }

};

#endif //TICKETSYSTEM_TOOL_HPP
