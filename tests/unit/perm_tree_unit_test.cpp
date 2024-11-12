#include "perm_tree.hpp"
#include <gtest/gtest.h>

TEST(Perm_tree_main, test_simple)
{
    perm_tree::perm_tree_t<int> tree;

    tree.insert(20);
    tree.insert(20);
    tree.insert(30);
    tree.insert(40);
    ASSERT_EQ(tree.detach_insert(5).size(), 2);
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