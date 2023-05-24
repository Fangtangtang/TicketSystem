#ifndef TICKETSYSTEM_MAP_HPP
#define TICKETSYSTEM_MAP_HPP
/**
 * implement a container like std::map
 */

#include <functional>
#include <cstddef>
#include <iostream>
#include "pair.hpp"
#include "exceptions.hpp"

namespace sjtu {

    //1.定义一个 type_traits 模板类
    template<typename T>
    struct my_type_traits {
        using iterator_assignable = typename T::iterator_assignable;//typename说明后面的是一个类型
    };


    //2.定义两个类，表示迭代器是否可被赋值的特性(这一步也可以使用枚举变量来实现)
    struct my_true_type {
        static constexpr bool value = true;
    };

    struct my_false_type {
        static constexpr bool value = false;
    };

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    >
    class map {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;
    private:
        struct node {
            value_type *data = nullptr;
            node *father = nullptr;
            node *left = nullptr;
            node *right = nullptr;
            int height = 0;//not an exact node if height==0

            node(int height_ = 1, node *father_ = nullptr, node *left_ = nullptr, node *right_ = nullptr)
                    : father(father_), left(left_), right(right_), height(height_) {}

            node(const value_type &valueType, int height_ = 1, node *father_ = nullptr, node *left_ = nullptr,
                 node *right_ = nullptr)
                    : father(father_), left(left_), right(right_), height(height_) {
                data = new value_type(valueType);
            }

            node(const Key &key_, int height_ = 1, node *father_ = nullptr) : height(height_), father(father_) {
                data = new value_type(key_, T());
            }

            node(node *other) {
                height = other->height;
                if (other->data) {
                    data = new value_type(*other->data);
                }
            }

            ~node() {
                delete data;
            }
        };

        //a fake node represent the root
        //the pointer root is its right son
        node root_node;
        size_t size_of_map = 0;
        node *end_node = nullptr, *begin_node = nullptr;//the exact begin_node and end_node
        Compare cmp;
        //a fake node (the right son of end)
        //insert the first value creat its node and a fake node as past_the_end
        //insert other one if insert at past_the_end,new(key value) and creat new past_the_end
        //creat its right son as past_the_end(without exact key value)
        node *past_the_end = nullptr;

    private:

        int getHeight(node *current_node) {
            if (current_node) return current_node->height;
            else return 0;
        }

        node *nextNode(node *current_node) {
            iterator iter(this, current_node);
            ++iter;
            return iter.getPos();
        }

        node *preNode(node *current_node) {
            iterator iter(this, current_node);
            --iter;
            return iter.getPos();
        }

        node *copyTree(node *other_node, const map &other) {
            node *tmp = new node(other_node);
            if (other_node == other.begin_node) begin_node = tmp;
            if (other_node == other.end_node) end_node = tmp;
            if (other_node->right) {
                tmp->right = copyTree(other_node->right, other);
                tmp->right->father = tmp;
            }
            if (other_node->left) {
                tmp->left = copyTree(other_node->left, other);
                tmp->left->father = tmp;
            }
            return tmp;
        }

        void deleteTree(node *current_root) {//the root is included
            if (current_root->right) deleteTree(current_root->right);
            if (current_root->left) deleteTree(current_root->left);
            delete current_root;
        }

        //recursive find
        //return false if key doesn't exist, the returned node is its father
        pair<node *, bool> findNode(const Key &key, node *subRoot) const {
            if (subRoot == past_the_end) return pair<node *, bool>(subRoot, false);
            if (!(cmp(key, subRoot->data->first) || cmp(subRoot->data->first, key))) {//find subRoot
                return pair<node *, bool>(subRoot, true);
            }
            if (cmp(key, subRoot->data->first)) {//possibly on its left son
                if (subRoot->left)
                    return findNode(key, subRoot->left);
                else return pair<node *, bool>(subRoot, false);
            }
            if (cmp(subRoot->data->first, key)) {
                if (subRoot->right)
                    return findNode(key, subRoot->right);
                else return pair<node *, bool>(subRoot, false);
            }
        }

        //return current root of the sub_tree
        node *LL(node *sub_root) {
            node *tmp = sub_root->left;//new root
            if (sub_root == sub_root->father->left)sub_root->father->left = tmp;
            else sub_root->father->right = tmp;
            tmp->father = sub_root->father;
            sub_root->father = tmp, sub_root->left = tmp->right;
            if (tmp->right) tmp->right->father = sub_root;
            tmp->right = sub_root;
            //update height
            sub_root->height =
                    1 + (getHeight(sub_root->left) > getHeight(sub_root->right) ?
                         getHeight(sub_root->left) : getHeight(sub_root->right));
            tmp->height = 1 + (getHeight(tmp->left) > getHeight(tmp->right) ?
                               getHeight(tmp->left) : getHeight(tmp->right));
            return tmp;
        }

        node *RR(node *sub_root) {
            node *tmp = sub_root->right;//new root
            if (sub_root == sub_root->father->left)sub_root->father->left = tmp;
            else sub_root->father->right = tmp;
            tmp->father = sub_root->father;
            sub_root->father = tmp, sub_root->right = tmp->left;
            if (tmp->left) tmp->left->father = sub_root;
            tmp->left = sub_root;
            //update height
            sub_root->height =
                    1 + (getHeight(sub_root->left) > getHeight(sub_root->right) ?
                         getHeight(sub_root->left) : getHeight(sub_root->right));
            tmp->height = 1 + (getHeight(tmp->left) > getHeight(tmp->right) ?
                               getHeight(tmp->left) : getHeight(tmp->right));
            return tmp;//update root
        }

        node *LR(node *sub_root) {
            RR(sub_root->left);
            return LL(sub_root);
        }

        node *RL(node *sub_root) {
            LL(sub_root->right);
            return RR(sub_root);
        }


        //deal with four situations
        node *adjust(node *sub_root) {
            int h1 = getHeight(sub_root->left), h2 = getHeight(sub_root->right);
            if (h1 > h2) {//left tree is higher
                if (getHeight(sub_root->left->left) > getHeight(sub_root->left->right))return LL(sub_root);
                else return LR(sub_root);
            } else {
                if (getHeight(sub_root->right->left) > getHeight(sub_root->right->right))return RL(sub_root);
                else return RR(sub_root);
            }
        }


        //update height upwards
        //if not balance adjust
        //stop until height of a root remain unchanged or update to root
        void updateTree(node *sub_root) {
            while (sub_root != &root_node) {
                int h1 = getHeight(sub_root->left), h2 = getHeight(sub_root->right);
                int pre_height = sub_root->height;
                if (h1 - h2 == 2 || h1 - h2 == -2) sub_root = adjust(sub_root);//not balance
                sub_root->height = 1 + (getHeight(sub_root->left) > getHeight(sub_root->right) ?
                                        getHeight(sub_root->left) : getHeight(sub_root->right));
                if (sub_root->height == pre_height) break;//height of a root remain unchanged
                sub_root = sub_root->father;//update upwards
            }
        }

        //insert a new node as leaf
        //with value_type
        node *insertNode(node *father, const value_type &value) {
            if (father == past_the_end) {//update end_node
                father->data = new value_type(value);
                end_node = father;
                end_node->right = new node(1, end_node);
                past_the_end = end_node->right;
                updateTree(father);
                return father;
            }
            if (cmp(value.first, father->data->first)) {//left son
                node *tmp = father->left = new node(value, 1, father);
                updateTree(father);
                if (father == begin_node) begin_node = father->left;
                return tmp;
            } else {
                node *tmp = father->right = new node(value, 1, father);
                updateTree(father);
                return tmp;
            }
        }

        //remove a leaf
        void removeNode(node *remove_node, node *replace_node = nullptr) {
            --size_of_map;
            node *father = remove_node->father;
            if (remove_node == father->left) father->left = replace_node;
            else father->right = replace_node;
            if (replace_node) replace_node->father = remove_node->father;
            delete remove_node;
            updateTree(father);
        }

        //exchange two node
        void exchange(node *remove_node, node *replace_node) {
            int tmp_height = remove_node->height;
            remove_node->height = replace_node->height;
            replace_node->height = tmp_height;
            if (remove_node->father == replace_node) {
                if (replace_node == replace_node->father->left)replace_node->father->left = remove_node;
                else replace_node->father->right = remove_node;
                remove_node->father = replace_node->father;
                node *tmp_left = replace_node->left;
                replace_node->left = remove_node->left;
                if (remove_node->left)replace_node->left->father = replace_node;
                replace_node->right = remove_node->right;
                if (replace_node->right)replace_node->right->father = replace_node;
                replace_node->father = remove_node;
                remove_node->left = tmp_left;
                if (tmp_left)tmp_left->father = remove_node;
                remove_node->right = replace_node;
                return;
            }
            if (replace_node->father == remove_node) {
                if (remove_node == remove_node->father->left)remove_node->father->left = replace_node;
                else remove_node->father->right = replace_node;
                replace_node->father = remove_node->father;
                node *tmp_right = remove_node->right;
                remove_node->left = replace_node->left;
                if (remove_node->left)remove_node->left->father = remove_node;
                remove_node->right = replace_node->right;
                if (remove_node->right)remove_node->right->father = remove_node;
                remove_node->father = replace_node;
                replace_node->left = remove_node;
                replace_node->right = tmp_right;
                if (tmp_right)tmp_right->father = replace_node;
                return;
            }
            if (remove_node == remove_node->father->left)remove_node->father->left = replace_node;
            else remove_node->father->right = replace_node;
            if (replace_node == replace_node->father->left)replace_node->father->left = remove_node;
            else replace_node->father->right = remove_node;
            node *tmp = remove_node->father;
            remove_node->father = replace_node->father;
            replace_node->father = tmp;
            node *A_left = remove_node->left, *A_right = remove_node->right, *B_left = replace_node->left, *B_right = replace_node->right;
            replace_node->left = A_left, replace_node->right = A_right;
            remove_node->left = B_left, remove_node->right = B_right;
            if (A_left)A_left->father = replace_node;
            if (A_right) A_right->father = replace_node;
            if (B_left)B_left->father = remove_node;
            if (B_right)B_right->father = remove_node;
        }

        //remove pre until find leaf
        void remove(node *remove_node) {
            //find leaf
            if (!remove_node->left && !remove_node->right) {
                removeNode(remove_node);
                return;
            }
            if (!remove_node->right) {//only have left son
                removeNode(remove_node, remove_node->left);
                return;
            }
            if (!remove_node->left && remove_node != end_node) {//only have real right tree
                removeNode(remove_node, remove_node->right);
                return;
            }
            node *replace_node = preNode(remove_node);//always exist
            exchange(remove_node, replace_node);
            remove(remove_node);
        }

    public:
        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        class const_iterator;

        class iterator {

        private:

            map<Key, T, Compare> *address = nullptr;//associate the iterator to a certain map
            node *current = nullptr;//the node the iterator point at

        public:
            using iterator_assignable = my_true_type;

            iterator(map<Key, T, Compare> *address_ = nullptr, node *current_ = nullptr) {
                address = address_;
                current = current_;
            }

            iterator(const iterator &other) {
                address = other.address;
                current = other.current;
            }

            void Assign(const T &value) {
                if (!current) {
                    std::cout << "Invalid!\n";
                    return;
                } else {
                    current->data->second = value;
                }
            }

            iterator operator++(int) {
                if (current == address->past_the_end) throw invalid_iterator();
                iterator tmp(*this);
                if (current->right) {//have right son
                    current = current->right;
                    while (current->left) {
                        current = current->left;
                    }
                } else {
                    if (current == current->father->left) {//the left son of father
                        current = current->father;
                    } else {//find the first father as the left son of its father
                        current = current->father;
                        while (current->father->left != current) {
                            current = current->father;
                        }
                        current = current->father;
                    }
                }
                return tmp;
            }

            iterator &operator++() {
                if (current == address->past_the_end) throw invalid_iterator();
                if (current->right) {//have right son
                    current = current->right;
                    while (current->left) {
                        current = current->left;
                    }
                } else {
                    if (current == current->father->left) {//the left son of father
                        current = current->father;
                    } else {//find the first father as the left son of its father
                        current = current->father;
                        while (current->father->left != current) {
                            current = current->father;
                        }
                        current = current->father;
                    }
                }
                return *this;
            }

            iterator operator--(int) {
                if (current == address->past_the_end) throw invalid_iterator();
                iterator tmp(*this);
                if (current->left) {//have left son
                    current = current->left;
                    while (current->right) {//the first one without right son
                        current = current->right;
                    }
                } else {
                    if (current == current->father->right) {//the right son of father
                        current = current->father;//father
                    } else {
                        current = current->father;
                        while (current->father->right != current) {//current is left son
                            current = current->father;
                        }
                        current = current->father;
                    }
                }
                return tmp;
            }

            iterator &operator--() {
                if (current == address->begin_node) throw invalid_iterator();
                if (current->left) {//have left son
                    current = current->left;
                    while (current->right) {
                        current = current->right;
                    }
                } else {
                    if (current == current->father->right) {//the right son of father
                        current = current->father;
                    } else {
                        current = current->father;
                        while (current->father->right != current) {
                            current = current->father;
                        }
                        current = current->father;
                    }
                }
                return *this;
            }

            value_type &operator*() const {
                return *current->data;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */

            bool operator==(const iterator &rhs) const {
                if (address != rhs.address) return false;
                if (current != rhs.current) return false;
                return true;
            }

            bool operator==(const const_iterator &rhs) const {
                if (address != rhs.getAddress()) return false;
                if (current != rhs.getPos()) return false;
                return true;
            }


            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                if (address != rhs.address) return true;
                if (current != rhs.current) return true;
                return false;
            }

            bool operator!=(const const_iterator &rhs) const {
                if (address != rhs.getAddress()) return true;
                if (current != rhs.getPos()) return true;
                return false;
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type *operator->() {
                return current->data;
            }

            node *getPos() const {
                return current;
            }

            map<Key, T, Compare> *getAddress() const {
                return address;
            }
        };

        class const_iterator {
            // it should have similar member method as iterator.
            //  and it should be able to construct from an iterator.

        private:
            // data members.
            const map<Key, T, Compare> *address = nullptr;//associate the iterator to a certain map
            node *current = nullptr;//the node the iterator point at

        public:
            using iterator_assignable = my_false_type;

            const_iterator(const map<Key, T, Compare> *address_ = nullptr, node *current_ = nullptr) {
                address = address_;
                current = current_;
            }

            const_iterator(const const_iterator &other) : address(other.address), current(other.current) {}

            const_iterator(const iterator &other) : address(other.getAddress()), current(other.getPos()) {}

            void Assign(const T &value) {}

            const_iterator operator++(int) {
                if (current == address->past_the_end) throw invalid_iterator();
                const_iterator tmp(*this);
                if (current->right) {//have right son
                    current = current->right;
                    while (current->left) {
                        current = current->left;
                    }
                } else {
                    if (current == current->father->left) {//the left son of father
                        current = current->father;
                    } else {//find the first father as the left son of its father
                        current = current->father;
                        while (current->father->left != current) {
                            current = current->father;
                        }
                        current = current->father;
                    }
                }
                return tmp;
            }

            const_iterator &operator++() {
                if (current == address->past_the_end) throw invalid_iterator();
                if (current->right) {//have right son
                    current = current->right;
                    while (current->left) {
                        current = current->left;
                    }
                } else {
                    if (current == current->father->left) {//the left son of father
                        current = current->father;
                    } else {//find the first father as the left son of its father
                        current = current->father;
                        while (current->father->left != current) {
                            current = current->father;
                        }
                        current = current->father;
                    }
                }
                return *this;
            }

            const_iterator operator--(int) {
                if (current == address->begin_node) throw invalid_iterator();
                const_iterator tmp(*this);
                if (current->left) {//have left son
                    current = current->left;
                    while (current->right) {
                        current = current->right;
                    }
                } else {
                    if (current == current->father->right) {//the right son of father
                        current = current->father;
                    } else {
                        current = current->father;
                        while (current->father->right != current) {
                            current = current->father;
                        }
                        current = current->father;
                    }
                }
                return tmp;
            }

            const_iterator &operator--() {
                if (current == address->begin_node) throw invalid_iterator();
                if (current->left) {//have left son
                    current = current->left;
                    while (current->right) {
                        current = current->right;
                    }
                } else {
                    if (current == current->father->right) {//the right son of father
                        current = current->father;
                    } else {
                        current = current->father;
                        while (current->father->right != current) {
                            current = current->father;
                        }
                        current = current->father;
                    }
                }
                return *this;
            }

            const value_type &operator*() const {
                return *current->data;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */

            bool operator==(const iterator &rhs) const {
                if (address != rhs.getAddress()) return false;
                if (current != rhs.getPos()) return false;
                return true;
            }

            bool operator==(const const_iterator &rhs) const {
                if (address != rhs.address) return false;
                if (current != rhs.current) return false;
                return true;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                if (address != rhs.getAddress()) return true;
                if (current != rhs.getPos()) return true;
                return false;
            }

            bool operator!=(const const_iterator &rhs) const {
                if (address != rhs.address) return true;
                if (current != rhs.current) return true;
                return false;
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            const value_type *operator->() const noexcept {
                return current->data;
            }

            const node *getPos() const {
                return current;
            }

            const map<Key, T, Compare> *getAddress() const {
                return address;
            }
        };

        map() : root_node(0, nullptr, nullptr, nullptr), cmp() {}

        map(const map &other) : root_node(0, nullptr, nullptr, nullptr), cmp() {
            if (other.size_of_map) {//other is not empty
                size_of_map = other.size_of_map;
                root_node.right = copyTree(other.root_node.right, other);
                root_node.right->father = &root_node;
                past_the_end = end_node->right;
            }
        }

        map &operator=(const map &other) {
            if (this == &other) return *this;
            if (size_of_map) {
                deleteTree(root_node.right);
            }
            size_of_map = other.size_of_map;
            root_node.right = begin_node = end_node = past_the_end = nullptr;
            if (other.size_of_map) {
                root_node.right = copyTree(other.root_node.right, other);
                root_node.right->father = &root_node;
//                if (other.past_the_end == other.end_node->right)
                past_the_end = end_node->right;
//                else past_the_end = end_node->father;
            }
            return *this;
        }

        ~map() {
            if (size_of_map)
                deleteTree(root_node.right);
        }

        template<class iteratorType>
        void assign(iteratorType iter, const T &value) {
            if (my_type_traits<iteratorType>::iterator_assignable::value == false) {
                std::cout << "Const iterator. Failed!\n";
                return;
            } else {
                iter.Assign(value);
            }
        }

        /**
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T &at(const Key &key) {
            if (!size_of_map)throw index_out_of_bound();//empty
            pair<node *, bool> tmp = findNode(key, root_node.right);
            if (!tmp.second) throw index_out_of_bound();
            else return tmp.first->data->second;
        }

        const T &at(const Key &key) const {
            if (!size_of_map)throw index_out_of_bound();//empty
            pair<node *, bool> tmp = findNode(key, root_node.right);
            if (!tmp.second) throw index_out_of_bound();
            else return tmp.first->data->second;
        }

        /**
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {
            if (!size_of_map) {
                ++size_of_map;
                root_node.right = new node(value_type(key, T()), 2, &root_node);
                root_node.right->right = new node(1, root_node.right);//a fake node
                end_node = begin_node = root_node.right;
                past_the_end = end_node->right;
                return end_node->data->second;
            }
            pair<node *, bool> tmp = findNode(key, root_node.right);
            if (tmp.second) return tmp.first->data->second;
            else {
                ++size_of_map;
                return insertNode(tmp.first, value_type(key, T()))->data->second;
            }
        }

        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const {
            if (!size_of_map)throw index_out_of_bound();//empty
            pair<node *, bool> tmp = findNode(key, root_node.right);
            if (!tmp.second) throw index_out_of_bound();
            else return tmp.first->data->second;
        }

        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            return iterator(this, begin_node);
        }

        const_iterator cbegin() const {
            return const_iterator(this, begin_node);
        }

        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return iterator(this, past_the_end);
        }

        const_iterator cend() const {
            return const_iterator(this, past_the_end);
        }

        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const {
            return size_of_map == 0;
        }


        /**
         * returns the number of elements.
         */
        size_t size() const {
            return size_of_map;
        }

        /**
         * clears the contents
         */
        void clear() {
            if (size_of_map) {
                deleteTree(root_node.right);
                size_of_map = 0;
                root_node.right = end_node = begin_node = past_the_end = nullptr;
            }
        }

        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            //insert the first ele
            if (!size_of_map) {
                ++size_of_map;
                root_node.right = new node(value, 2, &root_node);
                root_node.right->right = new node(1, root_node.right);//a fake node
                end_node = begin_node = root_node.right;
                past_the_end = end_node->right;
                return pair<iterator, bool>(iterator(this, root_node.right), true);
            }
            pair<node *, bool> tmp = findNode(value.first, root_node.right);
            if (tmp.second) return pair<iterator, bool>(iterator(this, tmp.first), false);
            ++size_of_map;
            return pair<iterator, bool>(iterator(this, insertNode(tmp.first, value)), true);
        }

        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if (pos.getAddress() != this || pos.getPos() == past_the_end || pos.getPos() == nullptr)
                throw invalid_iterator();
            if (size_of_map == 1) {
                clear();
                return;
            }
            node *remove_node = pos.getPos();
            if (remove_node == begin_node) {//special have no son or one right son
                if (remove_node->right) {
                    begin_node = remove_node->right;
                    removeNode(remove_node, remove_node->right);
                } else {
                    begin_node = nextNode(begin_node);
                    removeNode(remove_node);
                }
                return;
            }
            if (remove_node == end_node) {
                if (remove_node->left) {
                    end_node = preNode(end_node);
                    past_the_end->father = end_node;
                    end_node->right = past_the_end;
                    remove_node->right = nullptr;
                    removeNode(remove_node, remove_node->left);
                    updateTree(end_node);
                } else {
                    end_node = preNode(end_node);
                    removeNode(remove_node, past_the_end);
                }
                return;
            }
            remove(remove_node);
        }

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key &key) const {
            pair<node *, bool> tmp = findNode(key, root_node.right);
            if (tmp.second) return 1;
            else return 0;
        }

        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            pair<node *, bool> tmp = findNode(key, root_node.right);
            if (tmp.second) return iterator(this, tmp.first);
            else return iterator(this, past_the_end);
        }

        const_iterator find(const Key &key) const {
            pair<node *, bool> tmp = findNode(key, root_node.right);
            if (tmp.second) return const_iterator(this, tmp.first);
            else return const_iterator(this, past_the_end);
        }

        pair<iterator, bool> Find(const Key &key) {
            if (size_of_map == 0) return pair<iterator, bool>(iterator(), false);
            pair<node *, bool> tmp = findNode(key, root_node.right);
            if (tmp.second) return pair<iterator, bool>(iterator(this, tmp.first), true);
            else return pair<iterator, bool>(iterator(), false);
//            iterator(this, past_the_end);
        }
    };

}
#endif //TICKETSYSTEM_MAP_HPP
