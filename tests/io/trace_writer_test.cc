// File trace_writer_test.cc: tests for TraceWriter's output format.

#include "io/trace_writer.h"

#include <gtest/gtest.h>

#include <sstream>

TEST(TraceWriterTest, HeaderShowsTheWord) {
  std::ostringstream out;
  TraceWriter writer(out);
  writer.PrintHeader("abba");
  EXPECT_EQ(out.str(), "Word: abba\n");
}

TEST(TraceWriterTest, HeaderShowsEpsForTheEmptyWord) {
  std::ostringstream out;
  TraceWriter writer(out);
  writer.PrintHeader("");
  EXPECT_EQ(out.str(), "Word: eps\n");
}

TEST(TraceWriterTest, ConfigurationShowsStateAndEveryTape) {
  std::ostringstream out;
  TraceWriter writer(out);

  TMConfiguration configuration;
  configuration.state = "q1";
  configuration.tapes.emplace_back('.');
  configuration.tapes.emplace_back('.');
  configuration.tapes[0].LoadInitialContent("ab");

  writer.PrintConfiguration(0, configuration);
  EXPECT_EQ(out.str(), "(q1, tape1=[a]b, tape2=[.])\n");
}

TEST(TraceWriterTest, ConfigurationIsIndentedByStep) {
  std::ostringstream out;
  TraceWriter writer(out);

  TMConfiguration configuration;
  configuration.state = "q0";
  configuration.tapes.emplace_back('.');

  writer.PrintConfiguration(2, configuration);
  EXPECT_EQ(out.str(), "    (q0, tape1=[.])\n");
}

TEST(TraceWriterTest, TransitionShowsReadWriteAndMovesOfEveryTape) {
  std::ostringstream out;
  TraceWriter writer(out);

  TMTransition transition{"q0",
                          {'a', '.'},
                          "q1",
                          {'b', 'c'},
                          {Movement::kLeft, Movement::kStay}};
  writer.PrintTransition(0, transition);
  EXPECT_EQ(out.str(),
            "-> (q0, read a .) => (q1, write b c, move L S)\n");
}

TEST(TraceWriterTest, TransitionShowsRightMovementAsR) {
  std::ostringstream out;
  TraceWriter writer(out);

  TMTransition transition{"q0", {'a'}, "q0", {'a'}, {Movement::kRight}};
  writer.PrintTransition(0, transition);
  EXPECT_NE(out.str().find("move R"), std::string::npos);
}

TEST(TraceWriterTest, HaltedMessageIsIndented) {
  std::ostringstream out;
  TraceWriter writer(out);
  writer.PrintHalted(1);
  EXPECT_EQ(out.str(), "  no applicable transition; machine halts\n");
}

TEST(TraceWriterTest, ResultShowsVerdictAndFinalTape) {
  std::ostringstream accepted_out;
  TraceWriter accepted_writer(accepted_out);
  accepted_writer.PrintResult(true, ".[1]1.");
  EXPECT_EQ(accepted_out.str(), "Result: ACCEPTED  tape 1: .[1]1.\n\n");

  std::ostringstream rejected_out;
  TraceWriter rejected_writer(rejected_out);
  rejected_writer.PrintResult(false, "[b]");
  EXPECT_EQ(rejected_out.str(), "Result: REJECTED  tape 1: [b]\n\n");
}
