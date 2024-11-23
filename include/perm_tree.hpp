#pragma once

#include "avl_tree.hpp"

namespace perm_tree {
    using namespace avl_tree;

    template <typename KeyT, typename CompT = std::less<KeyT>>
    class perm_tree_t final : public avl_tree_t<KeyT, CompT> {
        using ascending_range   = typename avl_tree_t<KeyT, CompT>::ascending_range;
        using internal_iterator = typename avl_tree_t<KeyT, CompT>::internal_iterator;
        using list_nodes_t      = typename avl_tree_t<KeyT, CompT>::tree_nodes_buffer_t::list_nodes_t;
        using tree_node         = typename avl_tree_t<KeyT, CompT>::tree_node;

        using avl_tree_t<KeyT, CompT>::buffer_;

        avl_tree_t<KeyT, CompT>::tree_nodes_buffer_t branch_buffer_;
        tree_node* new_root_ = nullptr;

    private:
        std::list<KeyT> insert2new(const KeyT& key) {
            std::list<KeyT> path;
            const tree_node* main_root = avl_tree_t<KeyT, CompT>::get_root();
            if (!main_root)
                return path;

            new_root_ = branch_buffer_.add_node(main_root);

            internal_iterator current     = new_root_;
            internal_iterator destination = new_root_;
            while (current.is_valid()) {
                if (CompT()(key, current->key_)) {
                    path.push_back(current->key_);
                    if (current->left_) {
                        current->left_ = branch_buffer_.add_node(current->left_);
                        current = current->left_;
                    } else {
                        current->left_ = branch_buffer_.add_node(key);
                        destination = current->left_;
                        break;
                    }
                } else if (CompT()(current->key_, key)) {
                    path.push_back(current->key_);
                    if (current->right_) {
                        current->right_ = branch_buffer_.add_node(current->right_);
                        current = current->right_;
                    } else {
                        current->right_ = branch_buffer_.add_node(key);
                        destination = current->right_;
                        break;
                    }
                } else {
                    return path;
                }
            }

            switch2new();

            avl_tree_t<KeyT, CompT>::update_height (destination);
            avl_tree_t<KeyT, CompT>::update_Nchilds(destination);

#ifdef DEBUG
            print();
            std::cerr << "\n";
            branch_buffer_.print();
#endif

            for (auto& node : ascending_range{destination})
                avl_tree_t<KeyT, CompT>::balance(node, new_root_);

            return path;
        }

        void switch2new() {
            list_nodes_t& nodes = branch_buffer_.get_nodes();
            for (auto it = nodes.begin(), end = nodes.end(); it != end; ++it) {
                tree_node* node = it->get();
                if (node->left_)  node->left_->parent_  = node;
                if (node->right_) node->right_->parent_ = node;
            }
        }

        void switch2old() {
            list_nodes_t& nodes = branch_buffer_.get_nodes();
            for (auto it = nodes.begin(), end = nodes.end(); it != end; ++it) {
                tree_node* node = buffer_.get_node(it->get()->key_);
                if (!node)
                    continue;

                if (node->left_)  node->left_->parent_  = node;
                if (node->right_) node->right_->parent_ = node;
            }
        }

    public:
        perm_tree_t() {}

        perm_tree_t(const perm_tree_t<KeyT, CompT>& other) :
            avl_tree_t<KeyT, CompT>(static_cast<const avl_tree_t<KeyT, CompT>&>(other))
        {
            if (!other.new_root_)
                return;

            reset();

            const list_nodes_t& nodes = other.branch_buffer_.get_nodes();
            tree_node* parent  = nullptr;
            tree_node* current = nullptr;
            for (auto it = nodes.begin(), end = nodes.end(); it != end; ++it) {
                current = branch_buffer_.add_node(it->get());
                tree_node* node = buffer_.get_node(it->get()->key_);

                current->parent_ = parent;
                parent = current;

                if (!node)
                    continue;

                current->left_  = node->left_;
                current->right_ = node->right_;
            }
            new_root_ = branch_buffer_.front_ptr();
            branch_buffer_.print();
        }

        perm_tree_t<KeyT, CompT>& operator=(const perm_tree_t<KeyT, CompT>& other) {
            if (this == &other)
                return *this;

            perm_tree_t<KeyT, CompT> new_tree{other};
            avl_tree_t<KeyT, CompT>::operator=(static_cast<const avl_tree_t<KeyT, CompT>&>(other));
            branch_buffer_ = std::move(new_tree.branch_buffer_);
            new_root_      = std::move(new_tree.new_root_);
            return *this;
        }

        perm_tree_t(perm_tree_t<KeyT, CompT>&& other) noexcept :
            avl_tree_t<KeyT, CompT>(std::move(static_cast<avl_tree_t<KeyT, CompT>&>(other))),
            branch_buffer_(std::move(other.branch_buffer_)),
            new_root_     (std::move(other.new_root_)) {
            other.new_root_ = nullptr;
        }
        
        perm_tree_t& operator=(perm_tree_t<KeyT, CompT>&& other) noexcept {
            if (this == &other)
                return *this;

            avl_tree_t<KeyT, CompT>::operator=(std::move(static_cast<avl_tree_t<KeyT, CompT>&>(other)));
            std::swap(branch_buffer_, other.branch_buffer_);
            std::swap(new_root_,      other.new_root_);
            return *this;
        }
        
        std::ostream& print(std::ostream& os = std::cerr) const {
            switch2old();
            avl_tree_t<KeyT, CompT>::print(os);

            if (!new_root_)
                return os;

            switch2new();
            os << "\n\n";
            os << print_lblue("Detached tree with root = " << new_root_->key_ <<
                              "(" << new_root_ << ")" <<
                              ":\nkey(<child>, <child>, <parent>, <Nleft>, <Nright>,"
                                     "<height>, <ptr>, <parent ptr>):\n");

            avl_tree_t<KeyT, CompT>::print_subtree(os, new_root_);
            return os;
        }

        avl_tree_t<KeyT, CompT>::external_iterator insert(const KeyT& key) {
            attach();
            return avl_tree_t<KeyT, CompT>::insert(key);
        }

        std::list<KeyT> detach_insert(const KeyT& key) {
            attach();
            return insert2new(key);
        }

        void attach() {
            if (!new_root_)
                return;

            switch2old();
            avl_tree_t<KeyT, CompT>::insert(branch_buffer_.back_node().key_);
            reset();
        }

        void reset() {
            switch2old();
            branch_buffer_.clear();
            new_root_ = nullptr;
        }
    };

    template <typename KeyT, typename CompT>
    std::ostream& operator<<(std::ostream& os, const perm_tree_t<KeyT, CompT>& perm_tree) {
        return perm_tree.print(os);
    }
}