// File alphabet_test.cc: tests for Alphabet.

#include "model/alphabet.h"

#include <gtest/gtest.h>

TEST(AlphabetTest, NewAlphabetIsEmpty) {
  Alphabet alphabet;
  EXPECT_TRUE(alphabet.Empty());
  EXPECT_EQ(alphabet.Size(), 0u);
}

TEST(AlphabetTest, AddSymbolAddsAndContainsFindsIt) {
  Alphabet alphabet;
  EXPECT_TRUE(alphabet.AddSymbol('a'));
  EXPECT_TRUE(alphabet.Contains('a'));
  EXPECT_FALSE(alphabet.Contains('b'));
  EXPECT_EQ(alphabet.Size(), 1u);
  EXPECT_FALSE(alphabet.Empty());
}

TEST(AlphabetTest, AddingTheSameSymbolTwiceDoesNotGrowTheAlphabet) {
  Alphabet alphabet;
  alphabet.AddSymbol('a');
  alphabet.AddSymbol('a');
  EXPECT_EQ(alphabet.Size(), 1u);
}

TEST(AlphabetTest, HashIsReservedAndCannotBeAdded) {
  Alphabet alphabet;
  EXPECT_TRUE(Alphabet::IsReserved('#'));
  EXPECT_FALSE(alphabet.AddSymbol('#'));
  EXPECT_FALSE(alphabet.Contains('#'));
}

TEST(AlphabetTest, DotIsNotReservedUnlikeThePushdownAutomatonProject) {
  // Unlike the pushdown automaton project, '.' is the Turing machine's
  // own blank symbol and is commonly placed in Gamma, so it must be
  // free to be added here.
  Alphabet alphabet;
  EXPECT_FALSE(Alphabet::IsReserved('.'));
  EXPECT_TRUE(alphabet.AddSymbol('.'));
  EXPECT_TRUE(alphabet.Contains('.'));
}

TEST(AlphabetTest, IsSubsetOfTrueWhenEverySymbolBelongsToOther) {
  Alphabet sigma;
  sigma.AddSymbol('a');
  sigma.AddSymbol('b');
  Alphabet gamma;
  gamma.AddSymbol('a');
  gamma.AddSymbol('b');
  gamma.AddSymbol('X');

  EXPECT_TRUE(sigma.IsSubsetOf(gamma));
}

TEST(AlphabetTest, IsSubsetOfFalseWhenSomeSymbolIsMissing) {
  Alphabet sigma;
  sigma.AddSymbol('a');
  sigma.AddSymbol('c');
  Alphabet gamma;
  gamma.AddSymbol('a');
  gamma.AddSymbol('b');

  EXPECT_FALSE(sigma.IsSubsetOf(gamma));
}

TEST(AlphabetTest, EmptyAlphabetIsSubsetOfAnyAlphabet) {
  Alphabet empty;
  Alphabet gamma;
  gamma.AddSymbol('a');
  EXPECT_TRUE(empty.IsSubsetOf(gamma));
}
