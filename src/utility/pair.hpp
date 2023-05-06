#ifndef TICKETSYSTEM_PAIR_HPP
#define TICKETSYSTEM_PAIR_HPP

template<class T1, class T2>
class pair {
public:
    T1 first;
    T2 second;

    constexpr pair() : first(), second() {}//分别调用T1，T2的构造函数
    pair(const pair &other) = default;

    pair(pair &&other) = default;//移动赋值
    pair(const T1 &x, const T2 &y) : first(x), second(y) {}

    template<class U1, class U2>
    pair(U1 &&x, U2 &&y) : first(x), second(y) {}

    template<class U1, class U2>
    pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {}

    template<class U1, class U2>
    pair(pair<U1, U2> &&other) : first(other.first), second(other.second) {}
};

#endif //TICKETSYSTEM_PAIR_HPP
