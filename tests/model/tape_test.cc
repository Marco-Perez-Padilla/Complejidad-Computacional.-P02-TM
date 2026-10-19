// File tape_test.cc: tests for Tape, the tape infinite in both
// directions used by every Turing machine simulation.

#include "model/tape.h"

#include <gtest/gtest.h>

TEST(TapeTest, NewTapeIsASingleBlankCellWithTheHeadOnIt) {
  Tape tape('.');
  EXPECT_EQ(tape.Read(), '.');
  EXPECT_EQ(tape.HeadPosition(), 0);
  EXPECT_EQ(tape.ToString(), "[.]");
}

TEST(TapeTest, LoadInitialContentLeavesTheHeadOnTheFirstSymbol) {
  Tape tape('.');
  tape.LoadInitialContent("abc");
  EXPECT_EQ(tape.HeadPosition(), 0);
  EXPECT_EQ(tape.Read(), 'a');
  EXPECT_EQ(tape.ToString(), "[a]bc");
}

TEST(TapeTest, LoadingEmptyContentLeavesTheTapeBlank) {
  Tape tape('.');
  tape.LoadInitialContent("");
  EXPECT_EQ(tape.Read(), '.');
  EXPECT_EQ(tape.ToString(), "[.]");
}

TEST(TapeTest, WriteOverwritesTheSymbolUnderTheHead) {
  Tape tape('.');
  tape.LoadInitialContent("abc");
  tape.Write('X');
  EXPECT_EQ(tape.Read(), 'X');
  EXPECT_EQ(tape.ToString(), "[X]bc");
}

TEST(TapeTest, MoveRightAndLeftShiftTheHead) {
  Tape tape('.');
  tape.LoadInitialContent("abc");
  tape.Move(Movement::kRight);
  EXPECT_EQ(tape.Read(), 'b');
  EXPECT_EQ(tape.HeadPosition(), 1);
  tape.Move(Movement::kLeft);
  EXPECT_EQ(tape.Read(), 'a');
  EXPECT_EQ(tape.HeadPosition(), 0);
}

TEST(TapeTest, StayLeavesTheHeadWhereItIs) {
  Tape tape('.');
  tape.LoadInitialContent("abc");
  tape.Move(Movement::kRight);
  tape.Move(Movement::kStay);
  EXPECT_EQ(tape.HeadPosition(), 1);
  EXPECT_EQ(tape.Read(), 'b');
}

TEST(TapeTest, MovingPastTheRightEndReadsBlankAndExtendsTheTape) {
  Tape tape('.');
  tape.LoadInitialContent("ab");
  tape.Move(Movement::kRight);
  tape.Move(Movement::kRight);
  EXPECT_EQ(tape.Read(), '.');
  EXPECT_EQ(tape.HeadPosition(), 2);
  EXPECT_EQ(tape.ToString(), "ab[.]");
}

TEST(TapeTest, MovingPastTheLeftEndGoesToNegativePositions) {
  Tape tape('.');
  tape.LoadInitialContent("ab");
  tape.Move(Movement::kLeft);
  EXPECT_EQ(tape.Read(), '.');
  EXPECT_EQ(tape.HeadPosition(), -1);
  tape.Write('Z');
  EXPECT_EQ(tape.ToString(), "[Z]ab");
}

TEST(TapeTest, TheTapeIsInfiniteInBothDirections) {
  Tape tape('.');
  tape.LoadInitialContent("a");
  for (int i = 0; i < 100; ++i) tape.Move(Movement::kLeft);
  EXPECT_EQ(tape.HeadPosition(), -100);
  EXPECT_EQ(tape.Read(), '.');
  tape.Write('L');
  for (int i = 0; i < 200; ++i) tape.Move(Movement::kRight);
  EXPECT_EQ(tape.HeadPosition(), 100);
  EXPECT_EQ(tape.Read(), '.');
  tape.Write('R');
  for (int i = 0; i < 200; ++i) tape.Move(Movement::kLeft);
  EXPECT_EQ(tape.Read(), 'L');
}

TEST(TapeTest, WrittenCellsAreKeptWhenTheHeadComesBack) {
  Tape tape('.');
  tape.LoadInitialContent("ab");
  tape.Move(Movement::kLeft);
  tape.Write('Z');
  tape.Move(Movement::kRight);
  tape.Move(Movement::kLeft);
  EXPECT_EQ(tape.Read(), 'Z');
}

TEST(TapeTest, ToStringCoversEveryVisitedCellWithTheHeadInBrackets) {
  Tape tape('.');
  tape.LoadInitialContent("ab");
  tape.Move(Movement::kLeft);
  tape.Move(Movement::kRight);
  tape.Move(Movement::kRight);
  tape.Move(Movement::kRight);
  EXPECT_EQ(tape.ToString(), ".ab[.]");
}

TEST(TapeTest, BlankSymbolIsConfigurable) {
  Tape tape('_');
  EXPECT_EQ(tape.Read(), '_');
  tape.Move(Movement::kRight);
  EXPECT_EQ(tape.Read(), '_');
}
