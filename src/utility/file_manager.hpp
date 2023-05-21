/*
 * FILE_MANAGER
 * used to read and write file directly
 * all the ele stored are of same type
 *
 * different managers can be associated with the same file
 */

#ifndef TICKETSYSTEM_FILE_MANAGER_HPP
#define TICKETSYSTEM_FILE_MANAGER_HPP

#include <fstream>
#include <string>
#include "vector.hpp"

template<class ValueType>
class FileManager {
    static constexpr size_t value_size = sizeof(ValueType);
    std::fstream r_w_file;
    std::string file;
    int Num = 0;
    long pre_addr = 0;//previous writen address
public:
    explicit FileManager(const std::string &file_name) : file(file_name) {
        r_w_file.open(file_name);
        if (!r_w_file.good()) {//doesn't exist
            r_w_file.open(file_name, std::ios::out);
//            r_w_file.seekp(0);
//            r_w_file.write(reinterpret_cast<char *> (&pre_addr), sizeof(long));
            r_w_file.close();
            r_w_file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
//            std::cout<<file_name<<' '<<r_w_file.good()<<"@@@@@@@@@@@@\n";
        }
    }

    ~FileManager() {
        r_w_file.close();
    }

    /*
     * ReadEle
     * ------------------------------------------------------------------------------------------------------------
     * read one at address
     * read one with move
     */

    void ReadEle(const long &start_addr, ValueType &valueType) {
//        std::cout<<file<<' '<<r_w_file.good()<<"@@@@@@@@@@@@\n";

        r_w_file.seekg(start_addr);
        r_w_file.read(reinterpret_cast<char *> (&valueType), value_size);
    }

    void ReadEle(const long &start_addr, const int &move_num, ValueType &valueType) {
//        std::cout<<file<<' '<<r_w_file.good()<<"@@@@@@@@@@@@\n";

        r_w_file.seekg(start_addr + move_num * value_size);
        r_w_file.read(reinterpret_cast<char *> (&valueType), value_size);
    }

    void ReadEle(const long &start_addr, const int &num, sjtu::vector<ValueType> &vec) {
//        std::cout<<file<<' '<<r_w_file.good()<<"@@@@@@@@@@@@\n";

        ValueType valueType;
        for (int i = 0; i < num; ++i) {
            r_w_file.seekg(start_addr + i * value_size);
            r_w_file.read(reinterpret_cast<char *> (&valueType), value_size);
            vec.push_back(valueType);
        }
    }

    /*
     * WriteEle
     * ------------------------------------------------------------------------------------------------------------
     * write one at address
     * write one at end
     * write one with move
     * write num at end
     */
    long WriteEle(const long &start_addr, ValueType valueType) {
//        std::cout<<file<<' '<<r_w_file.good()<<"@@@@@1@@@@@@@\n";
//Num+=1;
        r_w_file.seekp(start_addr);
        pre_addr = r_w_file.tellp();
        r_w_file.write(reinterpret_cast<char *> (&valueType), value_size);
        return pre_addr;
    }

    long WriteEle(ValueType valueType) {
//        std::cout<<file<<' '<<r_w_file.good()<<"@@@@@2@@@@@@@\n";
        Num += 1;
        r_w_file.seekp(0, std::ios::end);
        pre_addr = r_w_file.tellp();
        r_w_file.write(reinterpret_cast<char *> (&valueType), value_size);
        return pre_addr;
    }

    long WriteEle(const long &start_addr, const int &move_num, ValueType valueType) {
//        std::cout<<file<<' '<<r_w_file.good()<<"@@@@3@@@@@@@@\n";

        r_w_file.seekp(start_addr + move_num * value_size);
        pre_addr = r_w_file.tellp();
        r_w_file.write(reinterpret_cast<char *> (&valueType), value_size);
        return pre_addr;
    }

    void WriteEle(ValueType valueType, const int &num) {
//        std::cout<<file<<' '<<r_w_file.good()<<"@@@@@4@@@@@@@\n";
        Num += num;
        r_w_file.seekp(0, std::ios::end);
        long addr = r_w_file.tellp();
        for (int i = 0; i < num; ++i) {
            r_w_file.seekp(addr + i * value_size);
            r_w_file.write(reinterpret_cast<char *> (&valueType), value_size);
        }
    }

    /*
     * GetAddress
     * ------------------------------------------------------------------------------------------------------------
     * return end address of file
     */
    long GetAddress() {
//        std::cout<<file<<' '<<r_w_file.good()<<"@@@@5@@@@@@@@\n";
        r_w_file.seekp(0, std::ios::end);
        return r_w_file.tellp();
    }

    long GetAddress(const long &start_addr, const int &interval) {
//        std::cout<<file<<' '<<r_w_file.good()<<"@@@@@6@@@@@@@\n";

        return start_addr + interval * value_size;
    }

    long GetPreAddress() {
        return pre_addr;
    }

    /*
     * MinValue
     * --------------------------------------------------------------------------------------------
     * return min value of the queried space
     */
    ValueType MinValue(const long &start_addr, const int &start, const int &end) {
        ValueType min_value, value;
        r_w_file.seekg(start_addr + start * value_size);
        r_w_file.read(reinterpret_cast<char *> (&min_value), value_size);
//        std::cout<<"\n====\n"<<min_value<<' ';
        for (int i = start + 1; i <= end; ++i) {
            r_w_file.seekg(start_addr + i * value_size);
            r_w_file.read(reinterpret_cast<char *> (&value), value_size);
//            std::cout<<value<<' ';
            min_value = std::min(min_value, value);
        }
//        std::cout<<"\n====\n";
        return min_value;
    }

    void Print() {
        std::cout << Num << " ## ";
        for (int i = 0; i < Num; ++i) {
            ValueType valueType;
            r_w_file.seekg(i * value_size);
            r_w_file.read(reinterpret_cast<char *> (&valueType), value_size);
            std::cout << valueType << ' ';
        }
        std::cout<<'\n';
    }

    void Check() {
        if (!r_w_file.good()) std::cout << "!!!!!!!!!!!!!!!!!!!!!!\n";
    }
};

#endif //TICKETSYSTEM_FILE_MANAGER_HPP
