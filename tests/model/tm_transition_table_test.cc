// File tm_transition_table_test.cc: tests for TMTransitionTable.

#include "model/tm_transition_table.h"

#include <gtest/gtest.h>

TEST(TMTransitionTableTest, NewTableIsEmpty) {
  TMTransitionTable table;
  EXPECT_EQ(table.Size(), 0u);
  EXPECT_TRUE(table.All().empty());
}

TEST(TMTransitionTableTest, AddStoresTheTransition) {
  TMTransitionTable table;
  table.Add({"q0", {'a'}, "q1", {'b'}, {Movement::kRight}});
  EXPECT_EQ(table.Size(), 1u);
  EXPECT_EQ(table.All()[0].from, "q0");
}

TEST(TMTransitionTableTest, FindReturnsTheMatchingTransition) {
  TMTransitionTable table;
  table.Add({"q0", {'a'}, "q1", {'b'}, {Movement::kRight}});
  table.Add({"q0", {'b'}, "q2", {'c'}, {Movement::kLeft}});

  auto found = table.Find("q0", {'b'});
  ASSERT_TRUE(found.has_value());
  EXPECT_EQ(found->to, "q2");
  EXPECT_EQ(found->write[0], 'c');
  EXPECT_EQ(found->move[0], Movement::kLeft);
}

TEST(TMTransitionTableTest, FindReturnsNulloptWhenNothingMatches) {
  TMTransitionTable table;
  table.Add({"q0", {'a'}, "q1", {'b'}, {Movement::kRight}});
  EXPECT_FALSE(table.Find("q0", {'z'}).has_value());
  EXPECT_FALSE(table.Find("q9", {'a'}).has_value());
}

TEST(TMTransitionTableTest, FindMatchesTheWholeTupleOfSymbolsOfAMultiTapeMachine) {
  TMTransitionTable table;
  table.Add({"q0",
             {'a', '.'},
             "q1",
             {'a', 'a'},
             {Movement::kRight, Movement::kRight}});
  EXPECT_TRUE(table.Find("q0", {'a', '.'}).has_value());
  EXPECT_FALSE(table.Find("q0", {'a', 'a'}).has_value());
  EXPECT_FALSE(table.Find("q0", {'.', 'a'}).has_value());
}

TEST(TMTransitionTableTest, StayMovementIsStoredAndReturned) {
  TMTransitionTable table;
  table.Add({"q0", {'a'}, "q1", {'a'}, {Movement::kStay}});
  EXPECT_EQ(table.Find("q0", {'a'})->move[0], Movement::kStay);
}
