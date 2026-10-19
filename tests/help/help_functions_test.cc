// File help_functions_test.cc: tests for the user-facing message
// functions and for command-line argument validation.

#include "help/help_functions.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace {

// Calls ValidateArguments with the given arguments (after the program
// name), capturing everything it prints.
struct Result {
  int status;
  Options options;
  std::string out;
  std::string err;
};

Result Validate(std::vector<std::string> args) {
  std::vector<std::string> storage = {"mt"};
  storage.insert(storage.end(), args.begin(), args.end());
  std::vector<char*> argv;
  for (std::string& arg : storage) argv.push_back(arg.data());

  Result result;
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();
  result.status = ValidateArguments(static_cast<int>(argv.size()), argv.data(),
                                    &result.options);
  result.err = testing::internal::GetCapturedStderr();
  result.out = testing::internal::GetCapturedStdout();
  return result;
}

}  // namespace

TEST(HelpFunctionsTest, HelpPrintsUsageSummary) {
  testing::internal::CaptureStdout();
  Help();
  std::string out = testing::internal::GetCapturedStdout();
  EXPECT_NE(out.find("-config"), std::string::npos);
  EXPECT_NE(out.find("-trace"), std::string::npos);
  EXPECT_NE(out.find("-in"), std::string::npos);
  EXPECT_NE(out.find("-out"), std::string::npos);
}

TEST(HelpFunctionsTest, UsageDescribesTheDefinitionFileFormat) {
  testing::internal::CaptureStdout();
  Usage();
  std::string out = testing::internal::GetCapturedStdout();
  EXPECT_NE(out.find("blank symbol"), std::string::npos);
  EXPECT_NE(out.find("number of tapes"), std::string::npos);
  EXPECT_NE(out.find("{L, R, S}"), std::string::npos);
}

TEST(HelpFunctionsTest, PrintWarningWritesWithPrefixToStderr) {
  testing::internal::CaptureStderr();
  PrintWarning("careful");
  EXPECT_EQ(testing::internal::GetCapturedStderr(), "Warning: careful\n");
}

TEST(HelpFunctionsTest, PrintErrorWritesWithPrefixToStderr) {
  testing::internal::CaptureStderr();
  PrintError("broken");
  EXPECT_EQ(testing::internal::GetCapturedStderr(), "Error: broken\n");
}

TEST(HelpFunctionsTest, ConfigAloneIsValid) {
  Result result = Validate({"-config", "m.txt"});
  EXPECT_EQ(result.status, -1);
  EXPECT_EQ(result.options.config_file, "m.txt");
  EXPECT_FALSE(result.options.trace);
  EXPECT_FALSE(result.options.input_file.has_value());
  EXPECT_FALSE(result.options.output_file.has_value());
}

TEST(HelpFunctionsTest, AllOptionsAreParsed) {
  Result result = Validate(
      {"-config", "m.txt", "-trace", "-in", "w.txt", "-out", "t.txt"});
  EXPECT_EQ(result.status, -1);
  EXPECT_TRUE(result.options.trace);
  EXPECT_EQ(*result.options.input_file, "w.txt");
  EXPECT_EQ(*result.options.output_file, "t.txt");
}

TEST(HelpFunctionsTest, OptionsCanComeInAnyOrder) {
  Result result = Validate({"-in", "w.txt", "-trace", "-config", "m.txt"});
  EXPECT_EQ(result.status, -1);
  EXPECT_EQ(result.options.config_file, "m.txt");
}

TEST(HelpFunctionsTest, HelpFlagsPrintHelpAndReturnZero) {
  for (const char* flag : {"--help", "-h"}) {
    Result result = Validate({flag});
    EXPECT_EQ(result.status, 0) << flag;
    EXPECT_NE(result.out.find("Turing machine simulator"), std::string::npos);
  }
}

TEST(HelpFunctionsTest, HelpTakesPriorityOverOtherProblems) {
  Result result = Validate({"-bogus", "--help"});
  EXPECT_EQ(result.status, 0);
}

TEST(HelpFunctionsTest, MissingConfigIsAnError) {
  Result result = Validate({"-trace"});
  EXPECT_EQ(result.status, 1);
  EXPECT_NE(result.err.find("-config <file> is required"), std::string::npos);
  EXPECT_NE(result.out.find("Command-line options"), std::string::npos);
}

TEST(HelpFunctionsTest, NoArgumentsAtAllIsAnError) {
  Result result = Validate({});
  EXPECT_EQ(result.status, 1);
}

TEST(HelpFunctionsTest, ConfigWithoutValueIsAnError) {
  Result result = Validate({"-config"});
  EXPECT_EQ(result.status, 1);
  EXPECT_NE(result.err.find("-config requires a file name"), std::string::npos);
}

TEST(HelpFunctionsTest, InWithoutValueIsAnError) {
  Result result = Validate({"-config", "m.txt", "-in"});
  EXPECT_EQ(result.status, 1);
  EXPECT_NE(result.err.find("-in requires a file name"), std::string::npos);
}

TEST(HelpFunctionsTest, OutWithoutValueIsAnError) {
  Result result = Validate({"-config", "m.txt", "-trace", "-out"});
  EXPECT_EQ(result.status, 1);
  EXPECT_NE(result.err.find("-out requires a file name"), std::string::npos);
}

TEST(HelpFunctionsTest, UnknownOptionIsAnError) {
  Result result = Validate({"-config", "m.txt", "-bogus"});
  EXPECT_EQ(result.status, 1);
  EXPECT_NE(result.err.find("unknown option '-bogus'"), std::string::npos);
}

TEST(HelpFunctionsTest, TypeOptionOfThePushdownProjectIsNoLongerAccepted) {
  Result result = Validate({"-config", "m.txt", "-type", "apv"});
  EXPECT_EQ(result.status, 1);
  EXPECT_NE(result.err.find("unknown option '-type'"), std::string::npos);
}

TEST(HelpFunctionsTest, OutWithoutTraceIsIgnoredWithAWarning) {
  Result result = Validate({"-config", "m.txt", "-out", "t.txt"});
  EXPECT_EQ(result.status, -1);
  EXPECT_FALSE(result.options.output_file.has_value());
  EXPECT_NE(result.err.find("-out has no effect without -trace"),
            std::string::npos);
}
