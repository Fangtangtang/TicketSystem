/*
 * toolClasses
 */
#ifndef TICKETSYSTEM_TOOL_HPP
#define TICKETSYSTEM_TOOL_HPP

#include <cstring>
#include <iostream>

struct CompareUsername;

class Username {
    char username[20] = {'\0'};
    friend CompareUsername;
public:
    Username() = default;

    explicit Username(char *username_);

};

Username::Username(char *username_) {
    memset(username, 0, sizeof(username));
    strcpy(username, username_);
}

struct CompareUsername {
    bool operator()(const Username &a, const Username &b) const {
        return strcmp(a.username, b.username) < 0;
    }

    int CompareStr(const Username &a, const Username &b) const {
        return strcmp(a.username, b.username);
    }
};

/*
 * Time class to manage data and time from June 1st to August 31st
 */
class Time {
    int minutes = 0;

public:
    Time() = default;

    Time(int hour, int minute, int month = 6, int day = 1) {
        if (month == 8)day += 61;
        if (month == 7)day += 30;
        --day;
        hour += day * 24;
        minutes = minute + hour * 60;
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

    int GetInterval(const Time &other) const {
        return minutes > other.minutes ? minutes - other.minutes : other.minutes - minutes;
    }

    void AddMinute(int delta) {
        minutes += delta;
    }

    void AddDay(int month, int day) {
        if (month == 8)day += 61;
        if (month == 7)day += 30;
        --day;
        minutes += day * 24 * 60;
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

#endif //TICKETSYSTEM_TOOL_HPP
