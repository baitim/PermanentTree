#pragma once

#include "avl_tree.hpp"

namespace perm_tree {
    using namespace avl_tree;

    template <typename KeyT, typename CompT = std::less<KeyT>>
    class perm_tree_t final : public avl_tree_t<KeyT, CompT> {
        using ascending_range     = typename avl_tree_t<KeyT, CompT>::ascending_range;
        using internal_iterator   = typename avl_tree_t<KeyT, CompT>::internal_iterator;
        using tree_node           = typename avl_tree_t<KeyT, CompT>::tree_node;

        using avl_tree_t<KeyT, CompT>::root_;

        avl_tree_t<KeyT, CompT>::tree_nodes_buffer_t branch_buffer_;
        tree_node* new_root_ = nullptr;

    private:
        std::list<KeyT> insert2new(const KeyT& key) {
            std::list<KeyT> path;
            if (!root_)
                return path;

            new_root_ = branch_buffer_.add_node(root_);

            internal_iterator current     = *new_root_;
            internal_iterator destination = *new_root_;
            while (current.is_valid()) {
                if (CompT()(key, current->key_)) {
                    path.push_back(current->key_);
                    if (current->left_) {
                        current = branch_buffer_.add_node(current->left_);
                    } else {
                        current->left_ = branch_buffer_.add_node(key);
                        current->left_->parent_ = std::addressof(*current);
                        destination = current->left_;
                        break;
                    }
                } else if (CompT()(current->key_, key)) {
                    path.push_back(current->key_);
                    if (current->right_) {
                        current = branch_buffer_.add_node(current->right_);
                    } else {
                        current->right_ = branch_buffer_.add_node(key);
                        current->right_->parent_ = std::addressof(*current);
                        destination = current->right_;
                        break;
                    }
                } else {
                    return path;
                }
            }

            for (auto& node : ascending_range{destination})
                avl_tree_t<KeyT, CompT>::balance(node);

            return path;
        }

    public:
        std::ostream& print(std::ostream& os) const {
            avl_tree_t<KeyT, CompT>::print(os);

            if (!new_root_)
                return os;

            os << "\n\n";
            os << print_lblue("Detached tree with root = " << new_root_->key_ <<
                            ":\nkey(<child>, <child>, <parent>, <Nleft>, <Nright>, <height>, <ptr>):\n");

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

            avl_tree_t<KeyT, CompT>::insert(branch_buffer_.back().key_);
            reset();
        }

        void reset() noexcept {
            branch_buffer_.clear();
            new_root_ = nullptr;
        }
    };

    template <typename KeyT, typename CompT>
    std::ostream& operator<<(std::ostream& os, const perm_tree_t<KeyT, CompT>& perm_tree) {
        return perm_tree.print(os);
    }
}