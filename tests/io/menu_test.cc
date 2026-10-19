// File menu_test.cc: tests for Menu, driven through istringstream /
// ostringstream instead of std::cin / std::cout.

#include "io/menu.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <sstream>

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

}  // namespace

TEST(MenuTest, ExitOptionEndsTheLoop) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream machine_out, trace;
  WordChecker checker(machine, false, machine_out, trace);
  std::istringstream in("3\n");
  std::ostringstream out;
  Menu menu(checker, in, out);

  menu.Run();

  EXPECT_NE(out.str().find("Choose an option"), std::string::npos);
}

TEST(MenuTest, EndOfInputEndsTheLoop) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream machine_out, trace;
  WordChecker checker(machine, false, machine_out, trace);
  std::istringstream in("");
  std::ostringstream out;
  Menu menu(checker, in, out);

  EXPECT_NO_THROW(menu.Run());
}

TEST(MenuTest, KeyboardModeChecksEveryWordUntilTheExitCommand) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream out, trace;
  WordChecker checker(machine, false, out, trace);
  std::istringstream in("1\n0\n00\n..\n3\n");
  Menu menu(checker, in, out);

  menu.Run();

  EXPECT_NE(out.str().find("'0': ACCEPTED"), std::string::npos);
  EXPECT_NE(out.str().find("'00': REJECTED"), std::string::npos);
}

TEST(MenuTest, KeyboardModeTreatsDotAsTheEmptyWord) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream out, trace;
  WordChecker checker(machine, false, out, trace);
  std::istringstream in("1\n.\n..\n3\n");
  Menu menu(checker, in, out);

  menu.Run();

  EXPECT_NE(out.str().find("'eps':"), std::string::npos);
}

TEST(MenuTest, KeyboardModeEndsWhenInputRunsOut) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream out, trace;
  WordChecker checker(machine, false, out, trace);
  std::istringstream in("1\n0\n");
  Menu menu(checker, in, out);

  EXPECT_NO_THROW(menu.Run());
}

TEST(MenuTest, FileModeChecksTheWordsOfTheGivenFile) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream out, trace;
  WordChecker checker(machine, false, out, trace);
  std::string path = "/tmp/tm_menu_test_words.txt";
  {
    std::ofstream file(path);
    file << "0\n00\n";
  }
  std::istringstream in("2\n" + path + "\n3\n");
  Menu menu(checker, in, out);

  menu.Run();

  EXPECT_NE(out.str().find("'0': ACCEPTED"), std::string::npos);
  EXPECT_NE(out.str().find("'00': REJECTED"), std::string::npos);
  std::remove(path.c_str());
}

TEST(MenuTest, FileModeWithMissingFileIsAWarningAndTheMenuContinues) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream out, trace;
  WordChecker checker(machine, false, out, trace);
  std::istringstream in("2\n/does/not/exist.txt\n3\n");
  Menu menu(checker, in, out);

  testing::internal::CaptureStderr();
  EXPECT_NO_THROW(menu.Run());
  std::string warning = testing::internal::GetCapturedStderr();

  EXPECT_NE(warning.find("Warning:"), std::string::npos);
  EXPECT_NE(warning.find("/does/not/exist.txt"), std::string::npos);
}

TEST(MenuTest, FileModeEndsWhenInputRunsOutBeforeTheFileName) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream out, trace;
  WordChecker checker(machine, false, out, trace);
  std::istringstream in("2\n");
  Menu menu(checker, in, out);

  EXPECT_NO_THROW(menu.Run());
}

TEST(MenuTest, InvalidOptionIsAWarningAndTheMenuContinues) {
  TuringMachine machine = MakeOddZeros();
  std::ostringstream out, trace;
  WordChecker checker(machine, false, out, trace);
  std::istringstream in("9\n3\n");
  Menu menu(checker, in, out);

  testing::internal::CaptureStderr();
  menu.Run();
  std::string warning = testing::internal::GetCapturedStderr();

  EXPECT_NE(warning.find("invalid menu option '9'"), std::string::npos);
}
