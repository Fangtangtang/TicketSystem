#ifndef TICKETSYSTEM_VECTOR_HPP
#define TICKETSYSTEM_VECTOR_HPP

#include "exceptions.hpp"

#include <iostream>
#include <climits>
#include <cstddef>
#include <ctime>
#include <cstdlib>
//size_t 无符号整数，做数组下标
namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 * 连续空间 随机存取
 */
    template<class T, class Compare=std::less <T>>
    class vector {
    private:
        T **storage = nullptr;//指针数组的数组名

        size_t size_of_vector = 0;//数组大小

        size_t end_of_vector = 0;//数组最后一个元素下一位下标

        Compare cmp;

        //在vector的storage开新的指针数组，原来的 指针数组暂归在tmp下
        //修改
        T **doubleSpace();//扩容函数

        //从数组的begin项开始到end项结束[begin,end)
        //storage接管tmp指向的空间
        //错位为step
        void copyDate(T **tmp, size_t begin, size_t end, int step = 0);

        int Random(int l, int r) {
            srand(time(NULL));
            return l + rand() % (r - l + 1);
        }

        void bubSort(int l, int r) {
            for (int i = l + 1; i <= r; ++i) {
                for (int j = i; j > 0 && cmp(*storage[j], *storage[j - 1]); j--)
                    std::swap(storage[j], storage[j - 1]);
            }
        }

        //sort the array based on cmp
        void sort(int l, int r) {

            if (l >= r) return;
            if (r - l < 10) {
                bubSort(l, r);
                return;
            }
            std::swap(storage[Random(l, r)], storage[l]);
//            T *at = storage[Random(l, r)];
//            storage[Random(l, r)] = storage[l];
//            storage[l] = at;
            T *tmp = storage[l];
            int left = l, right = r;
            do {
                while (cmp(*tmp, *storage[r]) && l < r) --r;
                if (l < r) {
                    storage[l] = storage[r];
                    ++l;
                }
                while (cmp(*storage[l], *tmp) && l < r) ++l;
                if (l < r) {
                    storage[r] = storage[l];
                    --r;
                }
            } while (l < r);
            storage[l] = tmp;
            sort(left, l - 1);
            sort(l + 1, right);
        }

    public:
        /**
         *
         * a type for actions of the elements of a vector, and you should write
         *   a class named const_iterator with same interfaces.
         */
        /**
         * you can see RandomAccessIterator at CppReference for help.
         */
        class const_iterator;//类声明

        class iterator {
            friend class vector<T, Compare>;
            // The following code is written for the C++ type_traits library.
            // Type traits is a C++ feature for describing certain properties of a type.
            // For instance, for an iterator, iterator::value_type is the type that the
            // iterator points to.
            // 对迭代器来说 value_type 是迭代器指向的类型
            // STL algorithms and containers may use these type_traits (e.g. the following
            // typedef) to work properly. In particular, without the following code,
            // @code{std::sort(iter, iter1);} would not compile.
            // See these websites for more information:
            // https://en.cppreference.com/w/cpp/header/type_traits
            // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
            // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        public:
            //重名 方便外面用
            using difference_type = std::ptrdiff_t;
            using value_type = T;//指向的类型
            using pointer = T *;//迭代器类型
            using reference = T &;//元素地址类型
            using iterator_category = std::output_iterator_tag;//标示迭代器类型

        private:
            /**
             *  add data members
             *   just add whatever you want.
             */
            size_t index = 0;//在vector指针数组中的下标
            vector<T, Compare> *vector_location = nullptr;//iterator对应的vector的地址

        public:
            iterator() = default;

            iterator(size_t pos = 0, vector<T, Compare> *location = nullptr) {
                index = pos;
                vector_location = location;
            }

            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             * 迭代器跨区间
             */
            iterator operator+(const int &n) const {
                vector<T, Compare>::iterator tmp(this->index, this->vector_location);
                tmp.index += n;
                return tmp;
            }

            iterator operator-(const int &n) const {
                vector<T, Compare>::iterator tmp(this);
                tmp.index += n;
                return tmp;
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invalid_iterator.
            int operator-(const iterator &rhs) const {
                if (vector_location != rhs.vector_location) throw invalid_iterator();
                return index - rhs.index;
            }

            iterator &operator+=(const int &n) {
                index += n;
                return *this;
            }

            iterator &operator-=(const int &n) {
                index -= n;
                return *this;
            }

            /**
             *  iter++
             */
            iterator operator++(int) {
                vector<T, Compare>::iterator tmp(this);
                ++this->index;
                return tmp;
            }

            /**
             *  ++iter
             */
            iterator &operator++() {
                ++this->index;
                return *this;
            }

            /**
             *  iter--
             */
            iterator operator--(int) {
                vector<T, Compare>::iterator tmp(this);
                --this->index;
                return tmp;
            }

            /**
             *  --iter
             */
            iterator &operator--() {
                --this->index;
                return *this;
            }

            /**
             *  *it
             * 解引用
             */
            T &operator*() const {
                return *vector_location->storage[index];
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                if (vector_location != rhs.vector_location) return false;
                if (index != rhs.index) return false;
                return true;
            }

            bool operator==(const const_iterator &rhs) const {
                if (vector_location != rhs.vector_location) return false;
                if (index != rhs.index) return false;
                return true;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                if (vector_location != rhs.vector_location) return true;
                if (index != rhs.index) return true;
                return false;
            }

            bool operator!=(const const_iterator &rhs) const {
                if (vector_location != rhs.vector_location) return true;
                if (index != rhs.index) return true;
                return false;
            }
        };

        /**
         *
         * has same function as iterator, just for a const object.
         * 指向的对象可以访问，不可以修改
         */
        class const_iterator {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;

        private:

            size_t index = 0;//在vector指针数组中的下标
            const vector<T, Compare> *vector_location = nullptr;//iterator对应的vector的地址

        public:
            const_iterator() = default;

            const_iterator(size_t pos = 0, const vector<T, Compare> *location = nullptr) {
                index = pos;
                vector_location = location;
            }

            const_iterator operator+(const int &n) const {
                vector<T, Compare>::const_iterator tmp(this);
                tmp.index += n;
                return tmp;
            }

            const_iterator operator-(const int &n) const {
                vector<T, Compare>::const_iterator tmp(this);
                tmp.index += n;
                return tmp;
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invalid_iterator.
            int operator-(const const_iterator &rhs) const {
                if (vector_location != rhs.vector_location) throw invalid_iterator();
                return index - rhs.index;
            }

            const_iterator &operator+=(const int &n) {
                index += n;
                return *this;
            }

            const_iterator &operator-=(const int &n) {
                index -= n;
                return *this;
            }

            /**
             *  iter++
             */
            const_iterator operator++(int) {
                vector<T, Compare>::const_iterator tmp(this);
                ++this->index;
                return tmp;
            }

            /**
             *  ++iter
             */
            const_iterator &operator++() {
                ++this->index;
                return *this;
            }

            /**
             *  iter--
             */
            const_iterator operator--(int) {
                vector<T, Compare>::const_iterator tmp(this);
                --this->index;
                return tmp;
            }

            /**
             *  --iter
             */
            const_iterator &operator--() {
                --this->index;
                return *this;
            }

            /**
             *  *it
             * 解引用
             */
            const T &operator*() const {
                return *vector_location->storage[index];
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                if (vector_location != rhs.vector_location) return false;
                if (index != rhs.index) return false;
                return true;
            }

            bool operator==(const const_iterator &rhs) const {
                if (vector_location != rhs.vector_location) return false;
                if (index != rhs.index) return false;
                return true;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                if (vector_location != rhs.vector_location) return true;
                if (index != rhs.index) return true;
                return false;
            }

            bool operator!=(const const_iterator &rhs) const {
                if (vector_location != rhs.vector_location) return true;
                if (index != rhs.index) return true;
                return false;
            }

        };

        /**
         *  Constructs
         * At least two: default constructor, copy constructor
         */
        vector() = default;

        vector(const vector &other);

        /**
         *  Destructor
         */
        ~vector();

        /**
         * Assignment operator
         * 赋值重载
         * 自己赋给自己？
         * 删掉原有
         * new新的
         */
        vector &operator=(const vector &other) {
            if (this == &other)return *this;
            //删
            for (int i = 0; i < end_of_vector; ++i) {
                delete storage[i];
            }
            delete[] storage;
            //新建
            size_of_vector = other.size_of_vector;
            end_of_vector = other.end_of_vector;
            storage = new T *[size_of_vector];//新的指针数组
            for (size_t i = 0; i < end_of_vector; ++i) {
                storage[i] = new T(*other.storage[i]);//直接调用拷贝构造
            }
            return *this;
        }

        void sort() {
            sort(0, end_of_vector - 1);
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         * 检查下标越界的赋值
         */
        T &at(const size_t &pos) {
            if (pos >= size_of_vector) throw index_out_of_bound();
            return *storage[pos];
        }

        const T &at(const size_t &pos) const {
            if (pos >= size_of_vector) throw index_out_of_bound();
            return *storage[pos];
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         * !!! Pay attentions
         *   In STL this operator does not check the boundary but I want you to do.
         */
        T &operator[](const size_t &pos) {
            if (pos >= size_of_vector) throw index_out_of_bound();
            return *storage[pos];
        }

        const T &operator[](const size_t &pos) const {
            if (pos >= size_of_vector) throw index_out_of_bound();
            return *storage[pos];
        }

        /**
         * access the first element.
         * throw container_is_empty if size == 0
         */
        const T &front() const {
            if (!size_of_vector) throw container_is_empty();
            return *storage[0];
        }

        /**
         * access the last element.
         * throw container_is_empty if size == 0
         */
        const T &back() const {
            if (!size_of_vector) throw container_is_empty();
            return *storage[end_of_vector - 1];
        }

        /**
         * returns an iterator to the beginning.
         * 检查是否为空？
         */
        iterator begin() {
            return iterator(0, this);//直接构造iter
        }

        const_iterator cbegin() const {
            return const_iterator(0, this);
        }

        /**
         * returns an iterator to the end.
         */
        iterator end() {
            return iterator(end_of_vector, this);
        }

        const_iterator cend() const {
            return const_iterator(end_of_vector, this);
        }

        /**
         * checks whether the container is empty
         */
        bool empty() const {
            if (end_of_vector == 0)return true;
            return false;
        }

        /**
         * returns the number of elements
         */
        size_t size() const {
            return end_of_vector;
        }

        /**
         * clears the contents
         */
        void clear() {
            for (int i = 0; i < end_of_vector; ++i) {
                delete storage[i];
            }
            delete[] storage;
            size_of_vector = 0;
            end_of_vector = 0;
        }

        /**
         * inserts value before pos
         * returns an iterator pointing to the inserted value.
         * pos合法吗
         */
        iterator insert(iterator pos, const T &value) {
            if (pos.vector_location != this) throw runtime_error();
            size_t index = pos.index;//待插入的下标
            if (end_of_vector == size_of_vector) {//拓展空间
                T **tmp = doubleSpace();//在vector的storage开新的指针数组，原来的 指针数组暂归在tmp下
                copyDate(tmp, 0, index);//0- (index-1)直接接管
                storage[index] = new T(value);//插入
                copyDate(tmp, index, end_of_vector, 1);//错位接管
                delete[] tmp;//删旧指针数组
            } else {
                size_t current_index = end_of_vector;
                while (current_index > index) {
                    storage[current_index] = storage[current_index - 1];
                    --current_index;
                }
                storage[index] = new T(value);
            }
            ++end_of_vector;
            return pos;
        }

        /**
         * inserts value at index ind.
         * after inserting, this->at(ind) == value
         * returns an iterator pointing to the inserted value.
         * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
         */
        iterator insert(const size_t &ind, const T &value) {
            if (ind > size_of_vector) throw index_out_of_bound();
            if (end_of_vector == size_of_vector) {//拓展空间
                T **tmp = doubleSpace();//在vector的storage开新的指针数组，原来的 指针数组暂归在tmp下
                copyDate(tmp, 0, ind - 1);//0- (index-1)直接接管
                storage[index] = new T(value);//插入
                copyDate(tmp, ind, end_of_vector - 1, 1);//错位接管
                delete[] tmp;//删旧指针数组
            } else {
                size_t current_index = end_of_vector;
                while (current_index > ind) {
                    storage[current_index] = storage[current_index - 1];
                    --current_index;
                }
                storage(index) = new T(value);
            }
            ++end_of_vector;
            return iterator(ind, this);
        }

        /**
         * removes the element at pos.
         * return an iterator pointing to the following element.
         * If the iterator pos refers the last element, the end() iterator is returned.
         */
        iterator erase(iterator pos) {
            if (pos.vector_location != this) throw runtime_error();
            size_t index = pos.index;
            --end_of_vector;
            T *tmp = storage[index];
            while (index < end_of_vector) {
                storage[index] = storage[index + 1];
                ++index;
            }
            delete tmp;
            return pos;
        }

        /**
         * removes the element with index ind.
         * return an iterator pointing to the following element.
         * throw index_out_of_bound if ind >= size
         */
        iterator erase(const size_t &ind) {
            if (ind >= size_of_vector) throw index_out_of_bound();
            --end_of_vector;
            size_t index = ind;
            T *tmp = storage[index];
            while (index < end_of_vector) {
                storage[index] = storage[1 + index];
                ++index;
            }
            delete tmp;
            return iterator(ind, this);
        }

        /**
         * adds an element to the end.
         */
        void push_back(const T &value) {
            if (end_of_vector == size_of_vector) {
                T **tmp = doubleSpace();
                copyDate(tmp, 0, end_of_vector);
                if (end_of_vector)delete[] tmp;
            }
            storage[end_of_vector] = new T(value);
            ++end_of_vector;
        }

        /**
         * remove the last element from the end.
         * throw container_is_empty if size() == 0
         */
        void pop_back() {
            if (!size()) throw container_is_empty();
            --end_of_vector;
            delete storage[end_of_vector];
        }
    };

    //拷贝构造函数
    template<class T, class Compare>
    vector<T, Compare>::vector(const vector &other) :cmp() {
        size_of_vector = other.size_of_vector;
        end_of_vector = other.end_of_vector;
        storage = new T *[size_of_vector];//新的指针数组
        for (size_t i = 0; i < end_of_vector; ++i) {
            storage[i] = new T(*other.storage[i]);//直接调用赋值构造
        }
    }

    //析构函数
    //先析构指针数组指向的对象
    //析构指针数组
    template<class T, class Compare>
    vector<T, Compare>::~vector() {
        for (int i = 0; i < end_of_vector; ++i) {
            delete storage[i];
        }
        delete[] storage;
    }

    template<class T, class Compare>
    T **vector<T, Compare>::doubleSpace() {
        T **tmp = storage;
        if (!size_of_vector) size_of_vector = 1;
        size_of_vector *= 2;
        storage = new T *[size_of_vector];
        return tmp;
    }

    //从数组的begin项开始到end项结束
    //storage接管tmp指向的空间
    //错位为step
    template<class T, class Compare>
    void vector<T, Compare>::copyDate(T **tmp, size_t begin, size_t end, int step) {
        for (size_t current_index = begin; current_index < end; ++current_index) {
            storage[current_index + step] = tmp[current_index];
        }
    }
}
#endif //TICKETSYSTEM_VECTOR_HPP
