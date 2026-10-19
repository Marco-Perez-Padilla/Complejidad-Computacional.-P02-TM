// File word_checker_test.cc: tests for WordChecker, using small
// hand-made machines and ostringstream instead of std::cout.

#include "io/word_checker.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
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

// Never halts.
TuringMachine MakeLooping() {
  TMTransitionTable t;
  t.Add(Step("q0", '.', "q0", '.', Movement::kRight));
  return TuringMachine({"q0"}, MakeAlphabet("a"), MakeAlphabet("a."), "q0",
                       '.', {}, 1, t);
}

std::string WriteTempFile(const std::string& name, const std::string& content) {
  std::string path = "/tmp/tm_word_checker_test_" + name + ".txt";
  std::ofstream file(path);
  file << content;
  return path;
}

}  // namespace

TEST(WordCheckerTest, CheckPrintsAcceptedWithTheResultingTape) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream out, trace;
  WordChecker checker(machine, false, out, trace);

  checker.Check("0");
  EXPECT_EQ(out.str(), "'0': ACCEPTED  tape: 0.[.]\n");
}

TEST(WordCheckerTest, CheckPrintsRejected) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream out, trace;
  WordChecker checker(machine, false, out, trace);

  checker.Check("00");
  EXPECT_NE(out.str().find("'00': REJECTED"), std::string::npos);
}

TEST(WordCheckerTest, DotIsTheEmptyWord) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream out, trace;
  WordChecker checker(machine, false, out, trace);

  checker.Check(".");
  EXPECT_NE(out.str().find("'eps': REJECTED"), std::string::npos);
}

TEST(WordCheckerTest, WordOutsideSigmaIsAWarningNotACrash) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream out, trace;
  WordChecker checker(machine, false, out, trace);

  testing::internal::CaptureStderr();
  EXPECT_NO_THROW(checker.Check("0x"));
  std::string warning = testing::internal::GetCapturedStderr();

  EXPECT_NE(warning.find("Warning:"), std::string::npos);
  EXPECT_NE(warning.find("not in Sigma"), std::string::npos);
  EXPECT_TRUE(out.str().empty());
}

TEST(WordCheckerTest, NonHaltingWordIsAWarningNotACrash) {
  TuringMachine machine = MakeLooping();
  std::ostringstream out, trace;
  WordChecker checker(machine, false, out, trace);

  testing::internal::CaptureStderr();
  EXPECT_NO_THROW(checker.Check("."));
  std::string warning = testing::internal::GetCapturedStderr();

  EXPECT_NE(warning.find("Warning:"), std::string::npos);
  EXPECT_NE(warning.find("infinite loop"), std::string::npos);
}

TEST(WordCheckerTest, TraceGoesToTheTraceStreamOnlyWhenEnabled) {
  TuringMachine machine = MakeOddZeros();

  std::ostringstream out_off, trace_off;
  WordChecker without_trace(machine, false, out_off, trace_off);
  without_trace.Check("0");
  EXPECT_TRUE(trace_off.str().empty());

  std::ostringstream out_on, trace_on;
  WordChecker with_trace(machine, true, out_on, trace_on);
  with_trace.Check("0");
  EXPECT_NE(trace_on.str().find("Word: 0"), std::string::npos);
  EXPECT_EQ(out_on.str().find("Word:"), std::string::npos);
}

TEST(WordCheckerTest, CheckAllFromFileChecksEveryNonEmptyLine) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream out, trace;
  WordChecker checker(machine, false, out, trace);
  std::string path = WriteTempFile("all", "0\n\n00\n101\n");

  checker.CheckAllFromFile(path);

  std::string output = out.str();
  EXPECT_NE(output.find("'0': ACCEPTED"), std::string::npos);
  EXPECT_NE(output.find("'00': REJECTED"), std::string::npos);
  EXPECT_NE(output.find("'101': ACCEPTED"), std::string::npos);
  std::remove(path.c_str());
}

TEST(WordCheckerTest, CheckAllFromFileSkipsInvalidWordsAndKeepsGoing) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream out, trace;
  WordChecker checker(machine, false, out, trace);
  std::string path = WriteTempFile("invalid", "0\n0z\n101\n");

  testing::internal::CaptureStderr();
  checker.CheckAllFromFile(path);
  std::string warning = testing::internal::GetCapturedStderr();

  EXPECT_NE(warning.find("0z"), std::string::npos);
  EXPECT_NE(out.str().find("'0': ACCEPTED"), std::string::npos);
  EXPECT_NE(out.str().find("'101': ACCEPTED"), std::string::npos);
  std::remove(path.c_str());
}

TEST(WordCheckerTest, CheckAllFromMissingFileThrowsFileNotFoundException) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream out, trace;
  WordChecker checker(machine, false, out, trace);

  EXPECT_THROW(checker.CheckAllFromFile("/does/not/exist.txt"),
               FileNotFoundException);
}
