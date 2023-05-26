/*
 * toolClasses
 */
#ifndef TICKETSYSTEM_TOOL_HPP
#define TICKETSYSTEM_TOOL_HPP

#include <cstring>
#include <iostream>
#include <utility>

enum STATUS {
    success, pending, refunded
};

class TransactionDetail;

class TrainSystem;

class TicketSystem;

class UserSystem;

class TransactionSystem;

class WaitingList;

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

    friend std::ostream &operator<<(std::ostream &os, const Username &information) {
        os << information.username << ' ';
        return os;
    }

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

    static int CompareStr(const Username &a, const Username &b) {
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
    Interval operator+(const int &delta) const {
        return Interval(minutes + delta);
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
class CompareExactTime;

class Time {
    int minutes = 0;
    friend CompareExactTime;
public:
    Time() = default;

    explicit Time(const int &a) : minutes(a) {}

    explicit Time(const std::string &str) {
        int para1 = (str[0] - '0') * 10 + str[1] - '0';
        int para2 = (str[3] - '0') * 10 + str[4] - '0';
        if (para1 < 6 || para1 > 8) {
            minutes = -1;
            return;
        } else if (para1 == 8) para2 += 61;
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
        if (a.minutes > b.minutes) {
            return 1 + (a.minutes - b.minutes) / 1440;
        }
        return 0;
    }

    friend std::ostream &operator<<(std::ostream &os, const Time &time_) {
        int month, day, hour, minute = time_.minutes;
        //convert
        hour = minute / 60;
        minute %= 60;
        day = hour / 24;
        hour %= 24;
        ++day;
        if (day > 92) {
            month = 9;
            day -= 92;
        } else if (day > 61) {
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
        if (day <= 9)os << 0;
        os << day << ' ';
        if (hour <= 9)os << 0;
        os << hour << ':';
        if (minute <= 9)os << 0;
        os << minute;
        return os;
    }

    bool Check() const {
        return minutes >= 0;
    }
    
    int Lag(const Time &other) const {
        return minutes > other.minutes ? (minutes / 1440 - other.minutes / 1440)
                                       : (other.minutes / 1440 - minutes / 1440);
    }

    int TimeLag(const Time &other) const {
        if (minutes >= other.minutes) {
            if (minutes % 1440 > other.minutes % 1440)
                return (minutes / 1440 - other.minutes / 1440) + 1;
            return (minutes / 1440 - other.minutes / 1440);
        } else {
            return 0;
        }
    }
    
    void AddDay(const int &day) {
        minutes += day * 1440;
    }

    Time Add(const int &day, const int &minute) const {
        return Time(minutes + day * 1440 + minute);
    }

    int IntervalMinutes(const Time &other) const {
        return minutes > other.minutes ? minutes - other.minutes : other.minutes - minutes;
    }
};

struct CompareExactTime {
    bool operator()(const Time &a, const Time &b) const {
        return a.minutes < b.minutes;
    }
};

const CompareExactTime compareExactTime;

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
    int seat_num = 0;
    Time start_sale;
    Time stop_sale;
    long station_addr = 0;
    long seat_addr = 0;
    char type = '\0';
    friend TrainSystem;
    friend TicketSystem;
public:
    Train() = default;

    Train(const int &stationNum_, const int &seat_num, const Time &start_sale_, const Time &stop_sale_,
          const char &type_,
          const long &station, const long &seat);

    friend std::ostream &operator<<(std::ostream &os, const Train &information);

};

Train::Train(const int &stationNum_, const int &seat_num, const Time &start_sale_, const Time &stop_sale_,
             const char &type_,
             const long &station, const long &seat) :
        stationNum(stationNum_), seat_num(seat_num), start_sale(start_sale_), stop_sale(stop_sale_), type(type_),
        station_addr(station), seat_addr(seat) {}

std::ostream &operator<<(std::ostream &os, const Train &information) {
    os << information.start_sale << ' ' << information.stop_sale << ' ' << information.stationNum << '\n';
    return os;
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
    friend TrainSystem;
    friend WaitingList;
public:
    Seat() = default;

    explicit Seat(const int &seat_num) : num(seat_num) {}

    bool operator<(const Seat &a) const;

    Seat operator+=(const int &delta);

    Seat operator-=(const int &delta);

    friend std::ostream &operator<<(std::ostream &os, const Seat &information);

};

const int SEAT_SIZE = sizeof(Seat);

bool Seat::operator<(const Seat &a) const {
    return num < a.num;
}

Seat Seat::operator+=(const int &delta) {
    num += delta;
    return *this;
}

Seat Seat::operator-=(const int &delta) {
    num -= delta;
    return *this;
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

class CompareTime;

class CompareTicketFromTo;

class CompareFromTicket;

class CompareToTicket;

class CompareTimeTicket;

class IsAvailableTicket;

class IsAvailableFrom;

class IsAvailableTo;


class Ticket {
    char from[31] = {'\0'};
    char to[31] = {'\0'};
    Time start_sale;//leaving date(exact time)
    Time stop_sale;

    friend TicketSystem;
    friend CompareTicket;
    friend CompareFrom;
    friend CompareTo;
    friend CompareTime;
    friend CompareTicketFromTo;
    friend CompareFromTicket;
    friend CompareToTicket;
    friend CompareTimeTicket;
    friend IsAvailableTicket;
    friend IsAvailableFrom;
    friend IsAvailableTo;
public:
    Ticket() = default;

    Ticket(const char *from_, const char *to_, const Time &start_sale_, const Time &stop_sale_);

    Ticket(const Time &start_sale_, const Time &stop_sale_);

    Ticket(const std::string &from_, const std::string &to_);

    Ticket(const std::string &from_, const std::string &to_, const Time &start_sale_, const Time &stop_sale_);

    bool operator<(const Ticket &a) const;

    friend std::ostream &operator<<(std::ostream &os, const Ticket &information);

};


Ticket::Ticket(const char *from_, const char *to_, const Time &start_sale_, const Time &stop_sale_) :
        start_sale(start_sale_), stop_sale(stop_sale_) {
    memset(from, 0, sizeof(from));
    strcpy(from, from_);
    memset(to, 0, sizeof(to));
    strcpy(to, to_);
}

Ticket::Ticket(const Time &start_sale_, const Time &stop_sale_) :
        start_sale(start_sale_), stop_sale(stop_sale_) {}

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

std::ostream &operator<<(std::ostream &os, const Ticket &information) {
    os << '\n';
    os << information.from << ' ' << information.to << '\n';
    os << information.start_sale << ' ' << information.stop_sale << '\n';
    return os;
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
 * CompareTime:
 *     used to insert in real time bpt
 *     ---------------------
 *     this tree is used to find
 *     which tickets are available on the given day
 *     ---------------------
 *     stop_sale start_sale to from *
 *
 * CompareTicketFromTo:
 *     from to stop_sale(cmp1 of query_ticket)
 *
 * CompareFromTicket:
 *     from stop_sale(cmp1 of fromTicketTree)
 *
 * CompareToTicket:
 *     to stop_sale(cmp1 of toTicketTree)
 *
 * CompareToTicket:
 *     stop_sale(cmp1 of timeTicketTree)
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
 * IsAvailableTime:
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

struct CompareTicketFromTo {
    bool operator()(const Ticket &a, const Ticket &b) const;
};

bool CompareTicketFromTo::operator()(const Ticket &a, const Ticket &b) const {
    int cmp = strcmp(a.from, b.from);
    if (cmp) return cmp < 0;
    return strcmp(a.to, b.to) < 0;
}

const CompareTicketFromTo compareTicketFromTo;

struct IsAvailableTicket {
    bool operator()(const Ticket &a, const Ticket &b) const;
};

//a:target
bool IsAvailableTicket::operator()(const Ticket &a, const Ticket &b) const {
    if (b.stop_sale < a.start_sale)return false;
    if (a.stop_sale < b.start_sale) return false;
    return true;
}

const IsAvailableTicket isAvailableTicket;

/*
 * Transaction class
 */
struct CompareTransStrict;
struct CompareTransWeak;
const CompareUsername compare_username;

class Transaction {
    Username username;
    int timestamp = 0;
    friend CompareTransStrict;
    friend CompareTransWeak;
    friend TransactionSystem;
public:
    Transaction() = default;

    Transaction(Username username_, int timestamp_);

    friend bool operator<(const Transaction &a, const Transaction &b);

    friend std::ostream &operator<<(std::ostream &os, const Transaction &information) {
        os << information.username << ' ' << information.timestamp << '\n';
        return os;
    }

};

Transaction::Transaction(Username username_, int timestamp_) : username(username_), timestamp(timestamp_) {}

bool operator<(const Transaction &a, const Transaction &b) {
    int cmp = compare_username.CompareStr(a.username, b.username);
    if (cmp) {
        return cmp < 0;
    }
    return a.timestamp < b.timestamp;
}

/*
 * used when store in file and strictFind
 */
struct CompareTransStrict {
    bool operator()(const Transaction &a, const Transaction &b) const {
        int cmp = compare_username.CompareStr(a.username, b.username);
        if (cmp) {
            return cmp < 0;
        }
        return a.timestamp < b.timestamp;
    }
};

const CompareTransStrict compareTransStrict;

/*
 * used in "index_based" find
 * use username as indexd
 */
struct CompareTransWeak {
    bool operator()(const Transaction &a, const Transaction &b) const {
        return compare_username(a.username, b.username);
    }
};

const CompareTransWeak compareTransWeak;

class TransactionSystem;

class TransactionDetail {
    TrainID trainID;
    char from[31] = {'\0'};
    char to[31] = {'\0'};
    Time leaving_time;
    Time arriving_time;
    int price = 0;
    int num = 0;
    STATUS status = success;
    long seat_address = 0;//store train_address in train_information file for faster read
    short start_seat{}, end_seat{};
    friend TransactionSystem;
    friend WaitingList;
public:
    TransactionDetail() = default;

    TransactionDetail(const TrainID &trainID_, const char *from_, const char *to_, const Time &leaving_time_,
                      const Time &arriving_time_,
                      const int &price_, const int &num_, const STATUS &status_, const long &seat_address_,
                      const short &start_seat, const short &end_seat);

    void ModifyStatus(const STATUS &status_) {
        status = status_;
    }

    friend std::ostream &operator<<(std::ostream &os, const TransactionDetail &information) {
        //status
        if (information.status == success) os << "[success] ";
        else if (information.status == pending) os << "[pending] ";
        else os << "[refunded] ";
        //stations
        os << information.trainID << ' ';
        os << information.from << ' ' << information.leaving_time;
        os << " -> ";
        os << information.to << ' ' << information.arriving_time << ' ';
        os << information.price << ' ' << information.num;
        return os;
    }
};

TransactionDetail::TransactionDetail(const TrainID &trainID_, const char *from_, const char *to_,
                                     const Time &leaving_time_, const Time &arriving_time_,
                                     const int &price_, const int &num_, const STATUS &status_,
                                     const long &seat_address_,
                                     const short &start_seat, const short &end_seat) :
        trainID(trainID_),
        leaving_time(leaving_time_),
        arriving_time(arriving_time_),
        price(price_),
        num(num_),
        status(status_),
        seat_address(seat_address_),
        start_seat(start_seat), end_seat(end_seat) {
    memset(from, 0, sizeof(from));
    strcpy(from, from_);
    memset(to, 0, sizeof(to));
    strcpy(to, to_);
}

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
