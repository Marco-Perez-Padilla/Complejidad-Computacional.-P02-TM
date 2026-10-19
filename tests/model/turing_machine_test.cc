// File turing_machine_test.cc: tests for TuringMachine, covering both
// its getters and every validation performed by its constructor.

#include "model/turing_machine.h"

#include <gtest/gtest.h>

#include "exceptions/exceptions.h"

namespace {

Alphabet MakeAlphabet(const std::string& symbols) {
  Alphabet alphabet;
  for (char symbol : symbols) alphabet.AddSymbol(symbol);
  return alphabet;
}

TMTransition Step(const std::string& from, char read, const std::string& to,
                  char write, Movement move) {
  return {from, {read}, to, {write}, {move}};
}

TuringMachine MakeValidMachine() {
  TMTransitionTable transitions;
  transitions.Add(Step("q0", 'a', "q1", 'b', Movement::kRight));
  return TuringMachine({"q0", "q1"}, MakeAlphabet("a"), MakeAlphabet("ab."),
                       "q0", '.', {"q1"}, 1, transitions);
}

// Builds a machine with one different thing wrong at a time.
TuringMachine Build(std::set<std::string> states, const std::string& sigma,
                    const std::string& gamma, const std::string& initial,
                    char blank, std::set<std::string> finals, int tapes,
                    TMTransitionTable transitions) {
  return TuringMachine(std::move(states), MakeAlphabet(sigma),
                       MakeAlphabet(gamma), initial, blank, std::move(finals),
                       tapes, std::move(transitions));
}

}  // namespace

TEST(TuringMachineTest, GettersReturnWhatWasPassedIn) {
  TuringMachine machine = MakeValidMachine();

  EXPECT_EQ(machine.States().size(), 2u);
  EXPECT_TRUE(machine.InputAlphabet().Contains('a'));
  EXPECT_TRUE(machine.TapeAlphabet().Contains('b'));
  EXPECT_EQ(machine.InitialState(), "q0");
  EXPECT_EQ(machine.BlankSymbol(), '.');
  EXPECT_EQ(machine.FinalStates().size(), 1u);
  EXPECT_EQ(machine.TapeCount(), 1);
  EXPECT_EQ(machine.Transitions().Size(), 1u);
}

TEST(TuringMachineTest, IsAcceptingChecksMembershipInF) {
  TuringMachine machine = MakeValidMachine();
  EXPECT_TRUE(machine.IsAccepting("q1"));
  EXPECT_FALSE(machine.IsAccepting("q0"));
  EXPECT_FALSE(machine.IsAccepting("unknown"));
}

TEST(TuringMachineTest, RejectsEmptyStateSet) {
  EXPECT_THROW(Build({}, "a", "a.", "q0", '.', {}, 1, {}),
               InvalidDefinitionException);
}

TEST(TuringMachineTest, RejectsInitialStateNotInQ) {
  EXPECT_THROW(Build({"q0"}, "a", "a.", "qX", '.', {}, 1, {}),
               InvalidDefinitionException);
}

TEST(TuringMachineTest, RejectsBlankSymbolNotInGamma) {
  EXPECT_THROW(Build({"q0"}, "a", "a", "q0", '.', {}, 1, {}),
               InvalidDefinitionException);
}

TEST(TuringMachineTest, RejectsBlankSymbolInSigma) {
  EXPECT_THROW(Build({"q0"}, "a.", "a.", "q0", '.', {}, 1, {}),
               InvalidDefinitionException);
}

TEST(TuringMachineTest, RejectsSigmaNotSubsetOfGamma) {
  EXPECT_THROW(Build({"q0"}, "ac", "a.", "q0", '.', {}, 1, {}),
               InvalidDefinitionException);
}

TEST(TuringMachineTest, RejectsFinalStateNotInQ) {
  EXPECT_THROW(Build({"q0"}, "a", "a.", "q0", '.', {"qX"}, 1, {}),
               InvalidDefinitionException);
}

TEST(TuringMachineTest, RejectsZeroOrNegativeTapeCount) {
  EXPECT_THROW(Build({"q0"}, "a", "a.", "q0", '.', {}, 0, {}),
               InvalidDefinitionException);
  EXPECT_THROW(Build({"q0"}, "a", "a.", "q0", '.', {}, -1, {}),
               InvalidDefinitionException);
}

TEST(TuringMachineTest, AcceptsAnEmptyFinalStateSet) {
  EXPECT_NO_THROW(Build({"q0"}, "a", "a.", "q0", '.', {}, 1, {}));
}

TEST(TuringMachineTest, RejectsTransitionWithUnknownOriginState) {
  TMTransitionTable transitions;
  transitions.Add(Step("qX", 'a', "q0", 'a', Movement::kRight));
  EXPECT_THROW(Build({"q0"}, "a", "a.", "q0", '.', {}, 1, transitions),
               InvalidDefinitionException);
}

TEST(TuringMachineTest, RejectsTransitionWithUnknownDestinationState) {
  TMTransitionTable transitions;
  transitions.Add(Step("q0", 'a', "qX", 'a', Movement::kRight));
  EXPECT_THROW(Build({"q0"}, "a", "a.", "q0", '.', {}, 1, transitions),
               InvalidDefinitionException);
}

TEST(TuringMachineTest, RejectsTransitionReadingSymbolOutsideGamma) {
  TMTransitionTable transitions;
  transitions.Add(Step("q0", 'z', "q0", 'a', Movement::kRight));
  EXPECT_THROW(Build({"q0"}, "a", "a.", "q0", '.', {}, 1, transitions),
               InvalidDefinitionException);
}

TEST(TuringMachineTest, RejectsTransitionWritingSymbolOutsideGamma) {
  TMTransitionTable transitions;
  transitions.Add(Step("q0", 'a', "q0", 'z', Movement::kRight));
  EXPECT_THROW(Build({"q0"}, "a", "a.", "q0", '.', {}, 1, transitions),
               InvalidDefinitionException);
}

TEST(TuringMachineTest, RejectsTransitionWithTheWrongNumberOfTapeEntries) {
  TMTransitionTable transitions;
  transitions.Add(Step("q0", 'a', "q0", 'a', Movement::kRight));
  // One entry per field, but the machine has two tapes.
  EXPECT_THROW(Build({"q0"}, "a", "a.", "q0", '.', {}, 2, transitions),
               InvalidDefinitionException);
}

TEST(TuringMachineTest, RejectsTwoConflictingTransitionsForTheSameStateAndRead) {
  TMTransitionTable transitions;
  transitions.Add(Step("q0", 'a', "q0", 'a', Movement::kRight));
  transitions.Add(Step("q0", 'a', "q0", '.', Movement::kRight));
  EXPECT_THROW(Build({"q0"}, "a", "a.", "q0", '.', {}, 1, transitions),
               InvalidDefinitionException);
}

TEST(TuringMachineTest, AcceptsAnExactDuplicateTransition) {
  TMTransitionTable transitions;
  transitions.Add(Step("q0", 'a', "q0", 'a', Movement::kRight));
  transitions.Add(Step("q0", 'a', "q0", 'a', Movement::kRight));
  EXPECT_NO_THROW(Build({"q0"}, "a", "a.", "q0", '.', {}, 1, transitions));
}

TEST(TuringMachineTest, AcceptsAMultiTapeMachine) {
  TMTransitionTable transitions;
  transitions.Add({"q0",
                   {'a', '.'},
                   "q0",
                   {'a', 'a'},
                   {Movement::kRight, Movement::kStay}});
  EXPECT_NO_THROW(Build({"q0"}, "a", "a.", "q0", '.', {}, 2, transitions));
}
