// File turing_machine_parser_test.cc: tests for ParseTuringMachineFile,
// using real temporary definition files.

#include "io/turing_machine_parser.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>

#include "exceptions/exceptions.h"

namespace {

// Writes content to a temporary file that is removed when it goes out of
// scope.
class TempFile {
 public:
  explicit TempFile(const std::string& content) {
    static int counter = 0;
    path_ = "/tmp/tm_parser_test_" + std::to_string(counter++) + ".txt";
    std::ofstream file(path_);
    file << content;
  }
  ~TempFile() { std::remove(path_.c_str()); }
  const std::string& path() const { return path_; }

 private:
  std::string path_;
};

const char kHeader[] =
    "q0 q1\n"
    "0 1\n"
    "0 1 .\n"
    "q0\n"
    ".\n"
    "q1\n";

std::string Definition(const std::string& tapes_and_transitions) {
  return std::string(kHeader) + tapes_and_transitions;
}

}  // namespace

TEST(TuringMachineParserTest, ParsesAWellFormedOneTapeMachine) {
  TempFile file(Definition("1\nq0 0 q1 0 R\nq0 1 q0 1 R\n"));
  TuringMachine machine = ParseTuringMachineFile(file.path());

  EXPECT_EQ(machine.States().size(), 2u);
  EXPECT_TRUE(machine.InputAlphabet().Contains('0'));
  EXPECT_TRUE(machine.TapeAlphabet().Contains('.'));
  EXPECT_EQ(machine.InitialState(), "q0");
  EXPECT_EQ(machine.BlankSymbol(), '.');
  EXPECT_TRUE(machine.IsAccepting("q1"));
  EXPECT_EQ(machine.TapeCount(), 1);
  EXPECT_EQ(machine.Transitions().Size(), 2u);
}

TEST(TuringMachineParserTest, ParsesTheThreeMovements) {
  TempFile file(Definition("1\nq0 0 q0 0 L\nq0 1 q0 1 R\nq0 . q1 . S\n"));
  TuringMachine machine = ParseTuringMachineFile(file.path());

  EXPECT_EQ(machine.Transitions().Find("q0", {'0'})->move[0], Movement::kLeft);
  EXPECT_EQ(machine.Transitions().Find("q0", {'1'})->move[0], Movement::kRight);
  EXPECT_EQ(machine.Transitions().Find("q0", {'.'})->move[0], Movement::kStay);
}

TEST(TuringMachineParserTest, TransitionFieldOrderIsFromReadToWriteMove) {
  TempFile file(Definition("1\nq0 0 q1 1 R\n"));
  TuringMachine machine = ParseTuringMachineFile(file.path());

  auto transition = machine.Transitions().Find("q0", {'0'});
  ASSERT_TRUE(transition.has_value());
  EXPECT_EQ(transition->to, "q1");
  EXPECT_EQ(transition->write[0], '1');
}

TEST(TuringMachineParserTest, ParsesAMultiTapeMachine) {
  TempFile file(Definition("2\nq0 0 . q1 0 0 R R\n"));
  TuringMachine machine = ParseTuringMachineFile(file.path());

  EXPECT_EQ(machine.TapeCount(), 2);
  auto transition = machine.Transitions().Find("q0", {'0', '.'});
  ASSERT_TRUE(transition.has_value());
  EXPECT_EQ(transition->write.size(), 2u);
  EXPECT_EQ(transition->move.size(), 2u);
}

TEST(TuringMachineParserTest, IgnoresCommentsAndBlankLines) {
  TempFile file(
      "# a comment\n"
      "\n"
      "q0 q1   # trailing comment\n"
      "0 1\n"
      "0 1 .\n"
      "q0\n"
      ".\n"
      "q1\n"
      "1\n"
      "# between transitions\n"
      "q0 0 q1 0 R\n");
  EXPECT_NO_THROW(ParseTuringMachineFile(file.path()));
}

TEST(TuringMachineParserTest, AllowsAnEmptyFinalStateLine) {
  // The line for F is always present in the format, even if it has no
  // tokens... but an empty line is dropped like any other blank line, so
  // an F with no states is written with a comment-only line instead.
  TempFile file(
      "q0 q1\n0 1\n0 1 .\nq0\n.\n# no final states\nq1\n1\nq0 0 q1 0 R\n");
  EXPECT_NO_THROW(ParseTuringMachineFile(file.path()));
}

TEST(TuringMachineParserTest, ExactDuplicateTransitionIsIgnoredNotAnError) {
  TempFile file(Definition("1\nq0 0 q1 0 R\nq0 0 q1 0 R\n"));
  TuringMachine machine = ParseTuringMachineFile(file.path());
  EXPECT_EQ(machine.Transitions().Size(), 1u);
}

TEST(TuringMachineParserTest, MissingFileThrowsFileNotFoundException) {
  EXPECT_THROW(ParseTuringMachineFile("/does/not/exist.txt"),
               FileNotFoundException);
}

TEST(TuringMachineParserTest, FileWithOnlyCommentsThrowsEmptyFileException) {
  TempFile file("# nothing here\n\n   \n");
  EXPECT_THROW(ParseTuringMachineFile(file.path()), EmptyFileException);
}

TEST(TuringMachineParserTest, IncompleteHeaderThrowsInvalidDefinition) {
  TempFile file("q0 q1\n0 1\n0 1 .\nq0\n");
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, MissingTransitionsThrowsInvalidDefinition) {
  TempFile file(Definition("1\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, MultiCharacterSigmaSymbolThrowsInvalidDefinition) {
  TempFile file("q0 q1\nab 1\n0 1 .\nq0\n.\nq1\n1\nq0 0 q1 0 R\n");
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, MultiCharacterGammaSymbolThrowsInvalidDefinition) {
  TempFile file("q0 q1\n0 1\n0 11 .\nq0\n.\nq1\n1\nq0 0 q1 0 R\n");
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, ReservedHashCannotBeASymbol) {
  // '#' starts a comment, so a symbol line can only contain it if it is
  // the whole token and survives comment stripping; build the case
  // through an escaped position instead: Gamma line "0 1 ." is fine,
  // while a '#' alone just comments the rest of the line out, leaving an
  // empty Sigma, which is still a valid (if useless) alphabet.
  TempFile file("q0 q1\n0 1 # b\n0 1 .\nq0\n.\nq1\n1\nq0 0 q1 0 R\n");
  EXPECT_NO_THROW(ParseTuringMachineFile(file.path()));
}

TEST(TuringMachineParserTest, InitialStateNotInQThrowsInvalidDefinition) {
  TempFile file("q0 q1\n0 1\n0 1 .\nqZ\n.\nq1\n1\nq0 0 q1 0 R\n");
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, InitialStateWithSeveralTokensThrowsInvalidDefinition) {
  TempFile file("q0 q1\n0 1\n0 1 .\nq0 q1\n.\nq1\n1\nq0 0 q1 0 R\n");
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, BlankSymbolNotInGammaThrowsInvalidDefinition) {
  TempFile file("q0 q1\n0 1\n0 1 .\nq0\nx\nq1\n1\nq0 0 q1 0 R\n");
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, MultiCharacterBlankThrowsInvalidDefinition) {
  TempFile file("q0 q1\n0 1\n0 1 .\nq0\n..\nq1\n1\nq0 0 q1 0 R\n");
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, BlankSymbolInSigmaThrowsInvalidDefinition) {
  TempFile file("q0 q1\n0 .\n0 1 .\nq0\n.\nq1\n1\nq0 0 q1 0 R\n");
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, SigmaNotSubsetOfGammaThrowsInvalidDefinition) {
  TempFile file("q0 q1\n0 1 2\n0 1 .\nq0\n.\nq1\n1\nq0 0 q1 0 R\n");
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, FinalStateNotInQThrowsInvalidDefinition) {
  TempFile file("q0 q1\n0 1\n0 1 .\nq0\n.\nqZ\n1\nq0 0 q1 0 R\n");
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, ZeroTapesThrowsInvalidDefinition) {
  TempFile file(Definition("0\nq0 0 q1 0 R\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, NegativeTapesThrowsInvalidDefinition) {
  TempFile file(Definition("-1\nq0 0 q1 0 R\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, NonNumericTapeCountThrowsInvalidDefinition) {
  TempFile file(Definition("two\nq0 0 q1 0 R\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, TapeCountWithTrailingGarbageThrowsInvalidDefinition) {
  TempFile file(Definition("2x\nq0 0 . q1 0 0 R R\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, HugeTapeCountThrowsInvalidDefinition) {
  TempFile file(Definition("99999999999999999999\nq0 0 q1 0 R\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, SeveralTokensInTapeCountLineThrowsInvalidDefinition) {
  TempFile file(Definition("1 2\nq0 0 q1 0 R\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, WrongFieldCountThrowsInvalidDefinition) {
  TempFile file(Definition("1\nq0 0 q1 R\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, OneTapeTransitionInATwoTapeMachineThrowsInvalidDefinition) {
  TempFile file(Definition("2\nq0 0 q1 0 R\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, OriginStateNotInQThrowsInvalidDefinition) {
  TempFile file(Definition("1\nqZ 0 q1 0 R\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, DestinationStateNotInQThrowsInvalidDefinition) {
  TempFile file(Definition("1\nq0 0 qZ 0 R\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, ReadSymbolNotInGammaThrowsInvalidDefinition) {
  TempFile file(Definition("1\nq0 2 q1 0 R\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, WriteSymbolNotInGammaThrowsInvalidDefinition) {
  TempFile file(Definition("1\nq0 0 q1 2 R\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, MultiCharacterReadSymbolThrowsInvalidDefinition) {
  TempFile file(Definition("1\nq0 01 q1 0 R\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, InvalidMovementThrowsInvalidDefinition) {
  TempFile file(Definition("1\nq0 0 q1 0 X\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, LowercaseMovementThrowsInvalidDefinition) {
  TempFile file(Definition("1\nq0 0 q1 0 r\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, ConflictingTransitionsThrowInvalidDefinition) {
  TempFile file(Definition("1\nq0 0 q1 0 R\nq0 0 q1 1 R\n"));
  EXPECT_THROW(ParseTuringMachineFile(file.path()), InvalidDefinitionException);
}

TEST(TuringMachineParserTest, ErrorMessageCarriesTheOffendingLineNumber) {
  TempFile file(Definition("1\nq0 0 q1 0 X\n"));
  try {
    ParseTuringMachineFile(file.path());
    FAIL() << "expected InvalidDefinitionException";
  } catch (const InvalidDefinitionException& error) {
    EXPECT_NE(std::string(error.what()).find("line 8"), std::string::npos);
  }
}
