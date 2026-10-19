// File simulator_test.cc: tests for Simulator, built directly on small
// hand-made machines (not through the parser, to keep each test focused
// on the simulation itself).

#include "simulation/simulator.h"

#include <gtest/gtest.h>

#include <sstream>

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

// Accepts binary strings with an odd number of 0s.
TuringMachine MakeOddZeros() {
  TMTransitionTable t;
  t.Add(Step("q0", '0', "q1", '0', Movement::kRight));
  t.Add(Step("q0", '1', "q0", '1', Movement::kRight));
  t.Add(Step("q1", '0', "q0", '0', Movement::kRight));
  t.Add(Step("q1", '1', "q1", '1', Movement::kRight));
  t.Add(Step("q1", '.', "q2", '.', Movement::kRight));
  return TuringMachine({"q0", "q1", "q2"}, MakeAlphabet("01"),
                       MakeAlphabet("01."), "q0", '.', {"q2"}, 1, t);
}

// Writes to the left of where the input started, so it only works on a
// tape that is infinite in both directions: 1^n -> 1^(2n).
TuringMachine MakeDouble() {
  TMTransitionTable t;
  t.Add(Step("q0", '1', "q1", 'a', Movement::kLeft));
  t.Add(Step("q1", '1', "q1", '1', Movement::kLeft));
  t.Add(Step("q1", 'a', "q1", 'a', Movement::kLeft));
  t.Add(Step("q1", '.', "q2", '1', Movement::kRight));
  t.Add(Step("q2", '1', "q2", '1', Movement::kRight));
  t.Add(Step("q2", 'a', "q3", 'a', Movement::kRight));
  t.Add(Step("q3", 'a', "q3", 'a', Movement::kRight));
  t.Add(Step("q3", '1', "q1", 'a', Movement::kLeft));
  t.Add(Step("q3", '.', "q4", '.', Movement::kLeft));
  t.Add(Step("q4", 'a', "q4", '1', Movement::kLeft));
  t.Add(Step("q4", '1', "q4", '1', Movement::kLeft));
  t.Add(Step("q4", '.', "q5", '.', Movement::kRight));
  return TuringMachine({"q0", "q1", "q2", "q3", "q4", "q5"}, MakeAlphabet("1"),
                       MakeAlphabet("1a."), "q0", '.', {"q5"}, 1, t);
}

// Never halts: bounces forever between two cells.
TuringMachine MakeLooping() {
  TMTransitionTable t;
  t.Add(Step("q0", '.', "q0", '.', Movement::kRight));
  return TuringMachine({"q0"}, MakeAlphabet("a"), MakeAlphabet("a."), "q0",
                       '.', {}, 1, t);
}

// Two tapes: copies tape 1 onto tape 2, then halts in q1.
TuringMachine MakeCopy() {
  TMTransitionTable t;
  t.Add({"q0", {'a', '.'}, "q0", {'a', 'a'}, {Movement::kRight, Movement::kRight}});
  t.Add({"q0", {'b', '.'}, "q0", {'b', 'b'}, {Movement::kRight, Movement::kRight}});
  t.Add({"q0", {'.', '.'}, "q1", {'.', '.'}, {Movement::kStay, Movement::kStay}});
  return TuringMachine({"q0", "q1"}, MakeAlphabet("ab"), MakeAlphabet("ab."),
                       "q0", '.', {"q1"}, 2, t);
}

// Uses the 'S' (stay) movement: replaces the first symbol and stays.
TuringMachine MakeStay() {
  TMTransitionTable t;
  t.Add(Step("q0", 'a', "q1", 'b', Movement::kStay));
  return TuringMachine({"q0", "q1"}, MakeAlphabet("a"), MakeAlphabet("ab."),
                       "q0", '.', {"q1"}, 1, t);
}

}  // namespace

TEST(SimulatorTest, AcceptsWordsOfTheLanguage) {
  TuringMachine machine = MakeOddZeros();
  Simulator simulator(machine);

  for (const char* word : {"0", "101", "10100", "000"}) {
    TMConfiguration result = simulator.Run(word);
    EXPECT_TRUE(machine.IsAccepting(result.state)) << word;
  }
}

TEST(SimulatorTest, RejectsWordsOutsideTheLanguage) {
  TuringMachine machine = MakeOddZeros();
  Simulator simulator(machine);

  for (const char* word : {"1", "00", "0000", "11"}) {
    TMConfiguration result = simulator.Run(word);
    EXPECT_FALSE(machine.IsAccepting(result.state)) << word;
  }
}

TEST(SimulatorTest, EmptyWordIsSimulatedFromABlankTape) {
  TuringMachine machine = MakeOddZeros();
  Simulator simulator(machine);

  // q0 has no transition on the blank, so the machine halts at once in
  // a non-final state.
  TMConfiguration result = simulator.Run("");
  EXPECT_EQ(result.state, "q0");
  EXPECT_FALSE(machine.IsAccepting(result.state));
}

TEST(SimulatorTest, HaltsOnTheConfigurationWhereNoTransitionApplies) {
  TuringMachine machine = MakeOddZeros();
  Simulator simulator(machine);

  TMConfiguration result = simulator.Run("0");
  EXPECT_EQ(result.state, "q2");
  EXPECT_EQ(result.tapes.size(), 1u);
}

TEST(SimulatorTest, TapeIsInfiniteToTheLeftAndTheResultIsOnTape1) {
  TuringMachine machine = MakeDouble();
  Simulator simulator(machine);

  TMConfiguration result = simulator.Run("111");
  EXPECT_TRUE(machine.IsAccepting(result.state));
  // 1^3 doubled is 1^6, written partly to the left of the input.
  EXPECT_EQ(result.tapes[0].ToString(), ".[1]11111.");
}

TEST(SimulatorTest, HeadEndsAtTheStartOfTheResult) {
  TuringMachine machine = MakeDouble();
  Simulator simulator(machine);

  TMConfiguration result = simulator.Run("11");
  EXPECT_EQ(result.tapes[0].Read(), '1');
  EXPECT_EQ(result.tapes[0].HeadPosition(), -2);
}

TEST(SimulatorTest, MultiTapeInputStartsOnlyOnTheFirstTape) {
  TuringMachine machine = MakeCopy();
  Simulator simulator(machine);

  TMConfiguration result = simulator.Run("abba");
  EXPECT_TRUE(machine.IsAccepting(result.state));
  ASSERT_EQ(result.tapes.size(), 2u);
  EXPECT_EQ(result.tapes[0].ToString(), "abba[.]");
  EXPECT_EQ(result.tapes[1].ToString(), "abba[.]");
}

TEST(SimulatorTest, StayMovementLeavesTheHeadInPlace) {
  TuringMachine machine = MakeStay();
  Simulator simulator(machine);

  TMConfiguration result = simulator.Run("a");
  EXPECT_TRUE(machine.IsAccepting(result.state));
  EXPECT_EQ(result.tapes[0].HeadPosition(), 0);
  EXPECT_EQ(result.tapes[0].ToString(), "[b]");
}

TEST(SimulatorTest, SymbolOutsideSigmaThrowsInvalidWordException) {
  TuringMachine machine = MakeOddZeros();
  Simulator simulator(machine);

  EXPECT_THROW(simulator.Run("012"), InvalidWordException);
}

TEST(SimulatorTest, BlankInTheWordIsOutsideSigma) {
  TuringMachine machine = MakeOddZeros();
  Simulator simulator(machine);

  EXPECT_THROW(simulator.Run("0.1"), InvalidWordException);
}

TEST(SimulatorTest, NonHaltingMachineThrowsExecutionLimitExceeded) {
  TuringMachine machine = MakeLooping();
  Simulator simulator(machine, /*max_steps=*/50);

  EXPECT_THROW(simulator.Run(""), ExecutionLimitExceededException);
}

TEST(SimulatorTest, MaxStepsBelowTheStepsNeededIsTreatedAsALoop) {
  TuringMachine machine = MakeOddZeros();
  Simulator simulator(machine, /*max_steps=*/1);

  EXPECT_THROW(simulator.Run("0000"), ExecutionLimitExceededException);
}

TEST(SimulatorTest, TraceWriterReceivesHeaderStepsAndResult) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream trace_out;
  TraceWriter writer(trace_out);
  Simulator simulator(machine, 10000, &writer);

  simulator.Run("0");

  std::string trace = trace_out.str();
  EXPECT_NE(trace.find("Word: 0"), std::string::npos);
  EXPECT_NE(trace.find("(q0, tape1=[0])"), std::string::npos);
  EXPECT_NE(trace.find("machine halts"), std::string::npos);
  EXPECT_NE(trace.find("Result: ACCEPTED"), std::string::npos);
}

TEST(SimulatorTest, TraceShowsEveryTapeOfAMultiTapeMachine) {
  TuringMachine machine = MakeCopy();
  std::ostringstream trace_out;
  TraceWriter writer(trace_out);
  Simulator simulator(machine, 10000, &writer);

  simulator.Run("ab");

  std::string trace = trace_out.str();
  EXPECT_NE(trace.find("tape1="), std::string::npos);
  EXPECT_NE(trace.find("tape2="), std::string::npos);
}

TEST(SimulatorTest, NoTraceIsWrittenWithoutATraceWriter) {
  TuringMachine machine = MakeOddZeros();
  Simulator simulator(machine);
  EXPECT_NO_THROW(simulator.Run("0"));
}
