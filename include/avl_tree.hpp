#pragma once

#include "ANSI_colors.hpp"
#include <iostream>
#include <list>
#include <cmath>
#include <memory>
#include <vector>

namespace avl_tree {
    
    template <typename KeyT, typename CompT = std::less<KeyT>>
    class avl_tree_t {
    protected:
        struct tree_node final {
            KeyT key_;
            int height_ = 1;
            int Nleft_  = 0;
            int Nright_ = 0;
            tree_node* parent_ = nullptr;
            tree_node* left_   = nullptr;
            tree_node* right_  = nullptr;

            tree_node(const KeyT& key) : key_(key) {}
            tree_node(const tree_node* node) : key_   (node->key_),    height_(node->height_),
                                               Nleft_ (node->Nleft_),  Nright_(node->Nright_),
                                               parent_(node->parent_), left_  (node->left_),
                                               right_ (node->right_) {}
        };

        class tree_nodes_buffer_t final {
            std::list<tree_node> nodes_;

        public:
            tree_node* add_node(const KeyT& key) {
                return std::addressof(nodes_.emplace_back(key));
            }

            tree_node* add_node(const tree_node* node) {
                if (!node)
                    return nullptr;
                return std::addressof(nodes_.emplace_back(node));
            }

            void print() {
                for (auto it : nodes_)
                    std::cout << it.key_ << "\n";
                std::cout << "\n";
            }

            void clear() noexcept {
                nodes_.clear();
            }

            tree_node& back() {
                return nodes_.back();
            }
        };

        class internal_iterator final {
            using iterator_category = std::forward_iterator_tag;
            using value_type        = tree_node;
            using pointer           = value_type*;
            using reference         = value_type&;
            using difference_type   = std::ptrdiff_t;

            pointer node_ = nullptr;

        public:
            internal_iterator() {}
            internal_iterator(const internal_iterator& node_it)  : node_(std::addressof(*node_it)) {}
            internal_iterator(reference node)                    : node_(std::addressof(node))     {}
            internal_iterator(pointer   node)                    : node_(node)                     {}

            internal_iterator& operator=(pointer node) {
                *this = internal_iterator{node};
                return *this;
            }

            bool is_valid() const noexcept { return (node_ != nullptr); }

            reference operator*() {
                if (node_) return *node_;
                throw std::invalid_argument("*nullptr");
            }

            const reference operator*() const {
                if (node_) return *node_;
                throw std::invalid_argument("*nullptr");
            }

            pointer operator->()             noexcept { return node_; }
            const pointer operator->() const noexcept { return node_; }

            bool operator==(const internal_iterator& rhs) const noexcept {
                return (rhs.node_ == node_);
            }

            bool operator!=(const internal_iterator& rhs) const noexcept {
                return !(rhs.node_ == node_);
            }

            internal_iterator& operator++() noexcept {
                if (is_valid())
                    node_ = node_->parent_;
                return *this;
            }
        };

        class ascending_range final {
            internal_iterator node_;

        public:
            ascending_range(const internal_iterator& node) : node_(node) {}
            internal_iterator begin() const { return node_; }
            internal_iterator end  () const { return internal_iterator{nullptr}; }
        };

    private:
        tree_nodes_buffer_t buffer_;
        tree_node* root_ = nullptr;

    public:
        class external_iterator final {
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type        = KeyT;
            using pointer           = const value_type*;
            using reference         = const value_type&;
            using difference_type   = std::ptrdiff_t;

            tree_node* node_;

        public:
            external_iterator(const internal_iterator& node) : node_(std::addressof(*node)) {}
            external_iterator(tree_node& node)               : node_(std::addressof(node))  {}
            external_iterator(tree_node* node)               : node_(node)       {}

            reference operator*() const {
                if (node_) return node_->key_;
                throw std::invalid_argument("nullptr->");
            }

            pointer operator->() const {
                if (node_) return std::addressof(node_->key_);
                throw std::invalid_argument("nullptr->");
            }

            bool operator==(const external_iterator& rhs) const noexcept {
                return (rhs.node_ == node_);
            }

            bool operator!=(const external_iterator& rhs) const noexcept {
                return !(rhs.node_ == node_);
            }

            external_iterator& operator++() noexcept {
                if (node_)
                    node_ = node_->parent_;
                return *this;
            }

            external_iterator& operator--() noexcept {
                if (node_)
                    node_ = node_->right_.get();
                return *this;
            }

            external_iterator operator--(int) noexcept {
                auto tmp = *this;
                if (node_)
                    node_ = node_->left_.get();
                return tmp;
            }
        };

    private:
        static void rotate_right(tree_node* node, tree_node*& root) {
            if (!node)
                return;

            tree_node* node_left_old = node->left_;

            node->left_ = node->left_->right_;
            node_left_old->right_ = node;

            if (node == root) {
                root = node_left_old;
            } else {
                if (CompT()(node->key_, node->parent_->key_))
                    node->parent_->left_  = node_left_old;
                else
                    node->parent_->right_ = node_left_old;
            }

            node_left_old->parent_ = node->parent_;
            if (node->left_)
                node->left_->parent_ = node;
            node->parent_ = node_left_old;

            update_height(node);
            update_Nchilds(node);
        }

        static void rotate_left(tree_node* node, tree_node*& root) {
            if (!node)
                return;

            tree_node* node_right_old = node->right_;

            node->right_ = node->right_->left_;
            node_right_old->left_ = node;

            if (node == root) {
                root = node_right_old;
            } else {
                if (CompT()(node->key_, node->parent_->key_))
                    node->parent_->left_  = node_right_old;
                else
                    node->parent_->right_ = node_right_old;
            }

            node_right_old->parent_ = node->parent_;
            if (node->right_)
                node->right_->parent_ = node;
            node->parent_ = node_right_old;
            
            update_height(node);
            update_Nchilds(node);
        }

        std::ostream& print_node(std::ostream& os, internal_iterator node) const noexcept {
            if (!node.is_valid())
                return os;

            os << print_lcyan(node->key_ << "\t(");

            if (node->left_)
                os << print_lcyan(node->left_->key_ << ",\t");
            else
                os << print_lcyan("none" << ",\t");

            if (node->right_)
                os << print_lcyan(node->right_->key_ << ",\t");
            else
                os << print_lcyan("none" << ",\t");

            if (node->parent_)
                os << print_lcyan(node->parent_->key_ << ",\t");
            else
                os << print_lcyan("none" << ",\t");

            os << print_lcyan(node->Nleft_  << ",\t" << node->Nright_ << ",\t" <<
                              node->height_ << ",\t" << std::addressof(*node) << ")\n");
            return os;
        }

        internal_iterator find(const KeyT& key) const {
            if (!root_)
                return 0;

            internal_iterator current = root_;
            while (current.is_valid()) {
                if (CompT()(key, current->key_)) {
                    if (current->left_)
                        current = current->left_;
                    else
                        return current;
                } else if (CompT()(current->key_, key)) {
                    if (current->right_)
                        current = current->right_;
                    else
                        return current;
                } else {
                    return current;
                }
            }
            return root_;
        }

    protected:
        static int get_node_size(internal_iterator node) noexcept {
            if (!node.is_valid())
                return 0;

            return node->Nleft_ + node->Nright_ + 1;
        }

        static void update_Nchilds(internal_iterator node) {
            for (auto& node_ : ascending_range{node}) {
                node_.Nleft_  = get_node_size(node_.left_);
                node_.Nright_ = get_node_size(node_.right_);
            }
        }

        static void update_height(internal_iterator node) {
            for (auto& node_ : ascending_range{node}) {
                node_.height_ = 0;
                if (node_.left_)
                    node_.height_ = node_.left_->height_;
                if (node_.right_)
                    node_.height_ = std::max(node_.height_, node_.right_->height_);

                node_.height_++;
            }
        }

        static void balance(internal_iterator node, tree_node*& root) {
            if (!node.is_valid())
                return;

            int balance_diff = 0;
            if (node->left_)
                balance_diff = node->left_->height_;
            if (node->right_)
                balance_diff -= node->right_->height_;

            int left_height = 0;
            int right_height = 0;
            if (balance_diff > 1) {
                tree_node* left = node->left_;
                if (left->left_)
                    left_height = left->left_->height_;

                if (left->right_)
                    right_height = left->right_->height_;

                if (left_height < right_height)
                    rotate_left(node->left_, root);
                rotate_right(std::addressof(*node), root);
                    
            } else if (balance_diff < -1) {
                tree_node* right = node->right_;
                if (right->left_)
                    left_height = right->left_->height_;

                if (right->right_)
                    right_height = right->right_->height_;

                if (left_height > right_height)
                    rotate_right(node->right_, root);
                rotate_left(std::addressof(*node), root);
            }
        }

        std::ostream& print_subtree(std::ostream& os, internal_iterator node) const {
            if (!node.is_valid())
                return os;

            int used_size = std::pow(2, std::log2(get_node_size(node)) + 1);
            std::vector<bool> used(used_size, false);

            internal_iterator current = node;
            int current_index = 1;
            while (current.is_valid()) {

                internal_iterator left_it = current->left_;
                if (left_it.is_valid() &&
                    !used[current_index * 2]) {
                    current = current->left_;
                    current_index = current_index * 2;
                    continue;
                }

                if (!used[current_index])
                    print_node(os, current);
                used[current_index] = true;

                internal_iterator right_it = current->right_;
                if (right_it.is_valid() &&
                    !used[current_index * 2 + 1]) {
                    current = current->right_;
                    current_index = current_index * 2 + 1;
                    continue;
                }

                current = current->parent_;
                current_index /= 2;
            }
            return os;
        }

    public:
        avl_tree_t() {}

        avl_tree_t(const avl_tree_t<KeyT, CompT>& other) {
            internal_iterator curr_other = other.root_;
            if (!curr_other.is_valid())
                return;

            root_ = buffer_.add_node(other.root_->key_);
            internal_iterator curr_this = root_;

            while (curr_other.is_valid()) {

                if (curr_other->left_ && !curr_this->left_) {
                    curr_other       = curr_other->left_;
                    curr_this->left_ = buffer_.add_node(curr_other->key_);
                    curr_this->left_->parent_ = std::addressof(*curr_this);
                    curr_this        = curr_this->left_;

                } else if (curr_other->right_ && !curr_this->right_) {
                    curr_other        = curr_other->right_;
                    curr_this->right_ = buffer_.add_node(curr_other->key_);
                    curr_this->right_->parent_ = std::addressof(*curr_this);
                    curr_this         = curr_this->right_;

                } else {
                    if (curr_other->parent_) {
                        update_height (curr_this);
                        update_Nchilds(curr_this);
                        curr_other = curr_other->parent_;
                        curr_this  = curr_this->parent_;
                    } else {
                        break;
                    }
                }
            }
        }

        avl_tree_t<KeyT, CompT>& operator=(const avl_tree_t<KeyT, CompT>& other) {
            if (this == &other)
                return *this;

            avl_tree_t<KeyT, CompT> new_tree{other};
            buffer_ = std::move(new_tree.buffer_);
            root_   = std::move(new_tree.root_);
            return *this;
        }

        avl_tree_t(avl_tree_t<KeyT, CompT>&& other) noexcept : buffer_ (std::move(other.buffer_)),
                                                               root_   (std::move(other.root_)) {
            other.root_ = nullptr;
        }
        
        avl_tree_t& operator=(avl_tree_t<KeyT, CompT>&& other) noexcept {
            if (this == &other)
                return *this;

            std::swap(buffer_, other.buffer_);
            std::swap(root_,   other.root_);
            return *this;
        }

        std::ostream& print(std::ostream& os) const {
            if (!root_)
                return os;

            os << print_lblue("Permanent tree with root = " << root_->key_ <<
                            ":\nkey(<child>, <child>, <parent>, <Nleft>, <Nright>, <height>, <ptr>):\n");

            print_subtree(os, root_);
            return os;
        }

        external_iterator insert(const KeyT& key) {
            tree_node* new_node = buffer_.add_node(key);

            if (!root_) {
                root_ = new_node;
                return root_;
            }

            internal_iterator current     = *root_;
            internal_iterator destination = *root_;
            while (current.is_valid()) {
                if (CompT()(key, current->key_)) {
                    if (current->left_) {
                        current = current->left_;
                    } else {
                        current->left_ = new_node;
                        current->left_->parent_ = std::addressof(*current);
                        destination = current->left_;
                        break;
                    }
                } else if (CompT()(current->key_, key)) {
                    if (current->right_) {
                        current = current->right_;
                    } else {
                        current->right_ = new_node;
                        current->right_->parent_ = std::addressof(*current);
                        destination = current->right_;
                        break;
                    }
                } else {
                    return current;
                }
            }

            update_height (destination);
            update_Nchilds(destination);

            for (auto& node_ : ascending_range{destination})
                balance(node_, root_);

            return find(destination->key_);
        }

        const tree_node* get_root() const { return const_cast<const tree_node*>(root_); }

        virtual ~avl_tree_t() {}
    };

    template <typename KeyT, typename CompT>
    std::ostream& operator<<(std::ostream& os, const avl_tree_t<KeyT, CompT>& avl_tree) {
        return avl_tree.print(os);
    }
}