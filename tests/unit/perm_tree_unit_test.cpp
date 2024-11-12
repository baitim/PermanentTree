#include "perm_tree.hpp"
#include <gtest/gtest.h>

void is_list_eq_vector(const std::list<int>& l, const std::vector<int>& v) {
    ASSERT_EQ(l.size(), v.size());

    int i = 0;
    for (auto l_it : l) {
        EXPECT_EQ(l_it, v[i]) << " at index: " << i << "\n";
        ++i;
    }
}

TEST(Perm_tree_main, test_simple)
{
    perm_tree::perm_tree_t<int> tree;

    tree.insert(20);
    tree.insert(20);
    tree.insert(30);
    tree.insert(40);
    ASSERT_EQ(tree.detach_insert(5).size(), 2);
}

TEST(Perm_tree_main, test_main)
{
    perm_tree::perm_tree_t<int> tree;
    std::list<int> ans;

    tree.insert(4);
    tree.insert(3);
    tree.insert(8);
    tree.insert(2);
    tree.insert(7);
    tree.insert(10);

    ans = tree.detach_insert(5);
    is_list_eq_vector(ans, {4, 8, 7});

    tree.reset();

    ans = tree.detach_insert(5);
    is_list_eq_vector(ans, {4, 8, 7});

    tree.insert(6);

    ans = tree.detach_insert(9);
    is_list_eq_vector(ans, {4, 8, 10});
}

TEST(Perm_tree_main, test_copy_ctor)
{   
    perm_tree::perm_tree_t<int> tree2;
    for (int i = 0; i < 10; i++) 
        tree2.insert(i);

    perm_tree::perm_tree_t<int> tree{tree2};

    EXPECT_EQ(tree.detach_insert(-1 ).size(), 3);
    EXPECT_EQ(tree.detach_insert( 15).size(), 4);
    EXPECT_EQ(tree.detach_insert( 6 ).size(), 3);
}

TEST(Perm_tree_raii, test_assign_ctor)
{
    perm_tree::perm_tree_t<int> tree;

    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(40);

    if (1) {
        perm_tree::perm_tree_t<int> tree2;
        tree2.insert(1);
        tree2.insert(2);
        tree2.insert(3);
        tree = tree2;
    }

    EXPECT_EQ(tree.detach_insert(2).size(), 0);
    EXPECT_EQ(tree.detach_insert(0).size(), 2);
    EXPECT_EQ(tree.detach_insert(4).size(), 2);
}

TEST(Perm_tree_raii, test_move_assign_ctor)
{
    perm_tree::perm_tree_t<int> tree;
    tree.insert(20);
    tree.insert(10);
    tree.insert(30);
    tree.insert(40);
    perm_tree::perm_tree_t<int> tree2;

    std::swap(tree, tree2);

    EXPECT_EQ(tree2.detach_insert(5 ).size(), 2);
    EXPECT_EQ(tree2.detach_insert(25).size(), 2);

    EXPECT_EQ(tree.detach_insert(5).size(), 0);
}