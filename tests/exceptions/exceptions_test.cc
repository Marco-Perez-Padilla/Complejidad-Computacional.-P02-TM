// File exceptions_test.cc: tests for the exception hierarchy.

#include "exceptions/exceptions.h"

#include <gtest/gtest.h>

#include <string>

TEST(ExceptionsTest, EveryExceptionDerivesFromTmException) {
  EXPECT_THROW(throw FileNotFoundException("f"), TmException);
  EXPECT_THROW(throw EmptyFileException("f"), TmException);
  EXPECT_THROW(throw InvalidArgumentsException("r"), TmException);
  EXPECT_THROW(throw InvalidDefinitionException(1, "r"), TmException);
  EXPECT_THROW(throw InvalidWordException("w", 'x'), TmException);
  EXPECT_THROW(throw ExecutionLimitExceededException("w"), TmException);
}

TEST(ExceptionsTest, TmExceptionIsAStdException) {
  EXPECT_THROW(throw TmException("m"), std::exception);
}

TEST(ExceptionsTest, FileNotFoundMessageNamesTheFile) {
  FileNotFoundException error("machine.txt");
  EXPECT_NE(std::string(error.what()).find("machine.txt"), std::string::npos);
}

TEST(ExceptionsTest, EmptyFileMessageNamesTheFile) {
  EmptyFileException error("machine.txt");
  EXPECT_NE(std::string(error.what()).find("machine.txt"), std::string::npos);
  EXPECT_NE(std::string(error.what()).find("empty"), std::string::npos);
}

TEST(ExceptionsTest, InvalidArgumentsMessageIncludesTheReason) {
  InvalidArgumentsException error("missing -config");
  EXPECT_NE(std::string(error.what()).find("missing -config"),
            std::string::npos);
}

TEST(ExceptionsTest, InvalidDefinitionMessageIncludesLineAndReason) {
  InvalidDefinitionException error(7, "initial state is not in Q");
  std::string message = error.what();
  EXPECT_NE(message.find("line 7"), std::string::npos);
  EXPECT_NE(message.find("initial state is not in Q"), std::string::npos);
}

TEST(ExceptionsTest, InvalidWordMessageIncludesWordAndSymbol) {
  InvalidWordException error("abx", 'x');
  std::string message = error.what();
  EXPECT_NE(message.find("abx"), std::string::npos);
  EXPECT_NE(message.find("'x'"), std::string::npos);
  EXPECT_NE(message.find("Sigma"), std::string::npos);
}

TEST(ExceptionsTest, ExecutionLimitMessageMentionsTheWordAndTheLoop) {
  ExecutionLimitExceededException error("aaa");
  std::string message = error.what();
  EXPECT_NE(message.find("aaa"), std::string::npos);
  EXPECT_NE(message.find("infinite loop"), std::string::npos);
}
