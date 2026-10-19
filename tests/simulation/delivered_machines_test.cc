// File delivered_machines_test.cc: end-to-end tests that load the
// machine definitions shipped in data/definitions/ (the two machines the
// assignment asks to design, plus the two given examples) and check
// their behavior. Paths are relative to the project root, which is where
// `make test` runs the test binary from.

#include <gtest/gtest.h>

#include "io/turing_machine_parser.h"
#include "simulation/simulator.h"

namespace {

std::string Tape1(const TuringMachine& machine, const std::string& word) {
  Simulator simulator(machine);
  return simulator.Run(word).tapes[0].ToString();
}

bool Accepts(const TuringMachine& machine, const std::string& word) {
  Simulator simulator(machine);
  return machine.IsAccepting(simulator.Run(word).state);
}

// The part of the tape from the head to the last non-blank cell.
std::string ResultFromHead(const TuringMachine& machine,
                           const std::string& word) {
  std::string tape = Tape1(machine, word);
  size_t open = tape.find('[');
  std::string result = tape.substr(open);
  result.erase(result.find(']'), 1);
  result.erase(0, 1);
  while (!result.empty() && result.back() == machine.BlankSymbol()) {
    result.pop_back();
  }
  return result;
}

}  // namespace

// ---- Machine 1: L = { a^n b^m | m >= n, n > 0 } ----

TEST(DeliveredMachinesTest, AnBmAcceptsTheLanguage) {
  TuringMachine machine = ParseTuringMachineFile("data/definitions/TM-anbm.txt");
  for (const char* word : {"ab", "aabb", "aabbb", "aaabbb", "abbbb", "aaabbbb"}) {
    EXPECT_TRUE(Accepts(machine, word)) << word;
  }
}

TEST(DeliveredMachinesTest, AnBmRejectsWhatIsOutsideTheLanguage) {
  TuringMachine machine = ParseTuringMachineFile("data/definitions/TM-anbm.txt");
  // m < n, n = 0, wrong order, and the empty word.
  for (const char* word : {"a", "aab", "aaabb", "b", "bb", "ba", "abab", "aba",
                           "abba", ""}) {
    EXPECT_FALSE(Accepts(machine, word)) << word;
  }
}

// ---- Machine 2: replace the word by #b in unary, blank, #a in unary ----

TEST(DeliveredMachinesTest, CountSymbolsMatchesTheAssignmentExample) {
  TuringMachine machine =
      ParseTuringMachineFile("data/definitions/TM-count_symbols.txt");
  // Assignment: abbabaabb -> 11111 (b) blank 1111 (a), head on the first
  // character of the result, nothing else left on the tape.
  EXPECT_EQ(ResultFromHead(machine, "abbabaabb"), "11111.1111");
  EXPECT_EQ(Tape1(machine, "abbabaabb").find('X'), std::string::npos);
}

TEST(DeliveredMachinesTest, CountSymbolsWritesZeroForAMissingSymbol) {
  TuringMachine machine =
      ParseTuringMachineFile("data/definitions/TM-count_symbols.txt");
  EXPECT_EQ(ResultFromHead(machine, "aa"), "0.11");
  EXPECT_EQ(ResultFromHead(machine, "bb"), "11.0");
}

TEST(DeliveredMachinesTest, CountSymbolsHandlesSingleSymbolsAndMixedOrders) {
  TuringMachine machine =
      ParseTuringMachineFile("data/definitions/TM-count_symbols.txt");
  EXPECT_EQ(ResultFromHead(machine, "a"), "0.1");
  EXPECT_EQ(ResultFromHead(machine, "b"), "1.0");
  EXPECT_EQ(ResultFromHead(machine, "ab"), "1.1");
  EXPECT_EQ(ResultFromHead(machine, "ba"), "1.1");
  EXPECT_EQ(ResultFromHead(machine, "baab"), "11.11");
  EXPECT_EQ(ResultFromHead(machine, "bbbaa"), "111.11");
  EXPECT_EQ(ResultFromHead(machine, "aaabb"), "11.111");
}

TEST(DeliveredMachinesTest, CountSymbolsHeadStartsOnTheFirstCharacterOfTheResult) {
  TuringMachine machine =
      ParseTuringMachineFile("data/definitions/TM-count_symbols.txt");
  Simulator simulator(machine);
  TMConfiguration result = simulator.Run("abbabaabb");
  EXPECT_EQ(result.tapes[0].Read(), '1');
  TMConfiguration only_a = simulator.Run("aa");
  EXPECT_EQ(only_a.tapes[0].Read(), '0');
}

// ---- The two examples given with the assignment ----

TEST(DeliveredMachinesTest, OddZerosExample) {
  TuringMachine machine =
      ParseTuringMachineFile("data/definitions/example-odd_zeros.txt");
  EXPECT_TRUE(Accepts(machine, "0"));
  EXPECT_TRUE(Accepts(machine, "101"));
  EXPECT_FALSE(Accepts(machine, "00"));
  EXPECT_FALSE(Accepts(machine, "1"));
}

TEST(DeliveredMachinesTest, DoubleUnaryExampleNeedsATapeInfiniteBothWays) {
  TuringMachine machine =
      ParseTuringMachineFile("data/definitions/example-double_unary.txt");
  for (int n = 1; n <= 8; ++n) {
    std::string word(static_cast<size_t>(n), '1');
    EXPECT_TRUE(Accepts(machine, word)) << n;
    EXPECT_EQ(ResultFromHead(machine, word),
              std::string(static_cast<size_t>(2 * n), '1'))
        << n;
  }
}

// ---- Multi-tape demonstration ----

TEST(DeliveredMachinesTest, CopyToTape2CopiesTheInputOntoTheSecondTape) {
  TuringMachine machine =
      ParseTuringMachineFile("data/definitions/example-copy_to_tape2.txt");
  Simulator simulator(machine);
  TMConfiguration result = simulator.Run("10110");
  ASSERT_EQ(result.tapes.size(), 2u);
  EXPECT_EQ(result.tapes[0].ToString(), result.tapes[1].ToString());
  EXPECT_EQ(result.tapes[1].ToString(), ".[1]0110.");
}
