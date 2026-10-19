// File application_test.cc: end-to-end tests for Application::Run,
// using real temporary machine/word files and istringstream/
// ostringstream instead of std::cin/std::cout.

#include "io/application.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <sstream>

#include "exceptions/exceptions.h"

namespace {

const char kOddZerosContent[] =
    "q0 q1 q2\n"
    "0 1\n"
    "0 1 .\n"
    "q0\n"
    ".\n"
    "q2\n"
    "1\n"
    "q0 0 q1 0 R\n"
    "q0 1 q0 1 R\n"
    "q1 0 q0 0 R\n"
    "q1 1 q1 1 R\n"
    "q1 . q2 . R\n";

std::string WriteTempFile(const std::string& name, const std::string& content) {
  std::string path = "/tmp/tm_application_test_" + name + ".txt";
  std::ofstream file(path);
  file << content;
  file.close();
  return path;
}

}  // namespace

TEST(ApplicationTest, RunWithInputFileChecksEveryWordAndPrintsSummary) {
  std::string machine_path = WriteTempFile("machine", kOddZerosContent);
  std::string words_path = WriteTempFile("words", "0\n00\n");

  Options options;
  options.config_file = machine_path;
  options.input_file = words_path;

  std::istringstream in;
  std::ostringstream out;
  Application application(options, in, out);

  application.Run();

  std::string output = out.str();
  EXPECT_NE(output.find("Machine loaded: 3 states, 1 tape(s), 1 accepting"),
            std::string::npos);
  EXPECT_NE(output.find("'0': ACCEPTED"), std::string::npos);
  EXPECT_NE(output.find("'00': REJECTED"), std::string::npos);

  std::remove(machine_path.c_str());
  std::remove(words_path.c_str());
}

TEST(ApplicationTest, RunWithoutInputFileGoesThroughTheInteractiveMenu) {
  std::string machine_path = WriteTempFile("machine_menu", kOddZerosContent);

  Options options;
  options.config_file = machine_path;

  std::istringstream in("1\n0\n..\n3\n");
  std::ostringstream out;
  Application application(options, in, out);

  application.Run();

  EXPECT_NE(out.str().find("Machine loaded"), std::string::npos);
  EXPECT_NE(out.str().find("'0': ACCEPTED"), std::string::npos);

  std::remove(machine_path.c_str());
}

TEST(ApplicationTest, RunWithMissingConfigFileThrowsFileNotFoundException) {
  Options options;
  options.config_file = "/does/not/exist.txt";

  std::istringstream in;
  std::ostringstream out;
  Application application(options, in, out);

  EXPECT_THROW(application.Run(), FileNotFoundException);
}

TEST(ApplicationTest, RunWithMalformedConfigFileThrowsInvalidDefinition) {
  std::string machine_path = WriteTempFile("machine_bad", "q0\n0\n");

  Options options;
  options.config_file = machine_path;

  std::istringstream in;
  std::ostringstream out;
  Application application(options, in, out);

  EXPECT_THROW(application.Run(), InvalidDefinitionException);
  std::remove(machine_path.c_str());
}

TEST(ApplicationTest, RunWithMissingInputFileThrowsFileNotFoundException) {
  std::string machine_path = WriteTempFile("machine_no_in", kOddZerosContent);

  Options options;
  options.config_file = machine_path;
  options.input_file = "/does/not/exist.txt";

  std::istringstream in;
  std::ostringstream out;
  Application application(options, in, out);

  EXPECT_THROW(application.Run(), FileNotFoundException);
  std::remove(machine_path.c_str());
}

TEST(ApplicationTest, RunWithTraceAndNoOutputFileWritesTheTraceToOut) {
  std::string machine_path = WriteTempFile("machine_trace_out", kOddZerosContent);
  std::string words_path = WriteTempFile("words_trace_out", "0\n");

  Options options;
  options.config_file = machine_path;
  options.input_file = words_path;
  options.trace = true;

  std::istringstream in;
  std::ostringstream out;
  Application application(options, in, out);

  application.Run();

  EXPECT_NE(out.str().find("Word: 0"), std::string::npos);

  std::remove(machine_path.c_str());
  std::remove(words_path.c_str());
}

TEST(ApplicationTest, RunWithTraceAndOutputFileWritesTheTraceToThatFile) {
  std::string machine_path = WriteTempFile("machine_trace", kOddZerosContent);
  std::string words_path = WriteTempFile("words_trace", "0\n");
  std::string trace_path = "/tmp/tm_application_test_trace_out.txt";

  Options options;
  options.config_file = machine_path;
  options.input_file = words_path;
  options.trace = true;
  options.output_file = trace_path;

  std::istringstream in;
  std::ostringstream out;
  Application application(options, in, out);

  application.Run();

  std::ifstream trace_file(trace_path);
  std::ostringstream trace_content;
  trace_content << trace_file.rdbuf();

  EXPECT_NE(trace_content.str().find("Word: 0"), std::string::npos);
  // The trace goes to the file, not to out.
  EXPECT_EQ(out.str().find("Word:"), std::string::npos);

  std::remove(machine_path.c_str());
  std::remove(words_path.c_str());
  std::remove(trace_path.c_str());
}

TEST(ApplicationTest, RunWithUnwritableOutputFileThrowsFileNotFoundException) {
  std::string machine_path = WriteTempFile("machine_bad_out", kOddZerosContent);

  Options options;
  options.config_file = machine_path;
  options.trace = true;
  options.output_file = "/";  // a directory can never be opened for writing

  std::istringstream in("3\n");
  std::ostringstream out;
  Application application(options, in, out);

  EXPECT_THROW(application.Run(), FileNotFoundException);

  std::remove(machine_path.c_str());
}

TEST(ApplicationTest, RunLoadsAMultiTapeMachine) {
  std::string machine_path = WriteTempFile(
      "machine_two_tapes",
      "q0 q1\n0 1\n0 1 .\nq0\n.\nq1\n2\n"
      "q0 0 . q0 0 0 R R\n"
      "q0 1 . q0 1 1 R R\n"
      "q0 . . q1 . . S S\n");
  std::string words_path = WriteTempFile("words_two_tapes", "010\n");

  Options options;
  options.config_file = machine_path;
  options.input_file = words_path;

  std::istringstream in;
  std::ostringstream out;
  Application application(options, in, out);

  application.Run();

  EXPECT_NE(out.str().find("2 tape(s)"), std::string::npos);
  EXPECT_NE(out.str().find("'010': ACCEPTED"), std::string::npos);

  std::remove(machine_path.c_str());
  std::remove(words_path.c_str());
}
