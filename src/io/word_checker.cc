/**
** Universidad de La Laguna
** Escuela Superior de Ingenieria y Tecnologia
** Grado en Ingenieria Informatica
** Subject: 'Complejidad Computacional'
** Course: 4º
** Practice 2: Turing machine
** Author: Marco Pérez Padilla
** Email: alu0101469348@ull.edu.es
** Date: 18/10/2026

** File word_checker.cc: implementation of WordChecker.
**/

#include "io/word_checker.h"

#include <fstream>

#include "exceptions/exceptions.h"
#include "help/help_functions.h"
#include "io/trace_writer.h"
#include "simulation/simulator.h"

namespace {
constexpr const char kEmptyWordToken[] = ".";
}  // namespace

WordChecker::WordChecker(const TuringMachine& machine, bool trace,
                          std::ostream& out, std::ostream& trace_out)
    : machine_(machine), trace_(trace), out_(out), trace_out_(trace_out) {}

void WordChecker::Check(const std::string& word) const {
  const std::string actual_word = word == kEmptyWordToken ? "" : word;

  TraceWriter writer(trace_out_);
  Simulator simulator(machine_, 10000, trace_ ? &writer : nullptr);
  try {
    TMConfiguration result = simulator.Run(actual_word);
    bool accepted = machine_.IsAccepting(result.state);
    out_ << "'" << (actual_word.empty() ? "eps" : actual_word) << "': "
         << (accepted ? "ACCEPTED" : "REJECTED") << "  tape: "
         << result.tapes[0].ToString() << "\n";
  } catch (const InvalidWordException& e) {
    PrintWarning(e.what());
  } catch (const ExecutionLimitExceededException& e) {
    PrintWarning(e.what());
  }
}

void WordChecker::CheckAllFromFile(const std::string& filename) const {
  std::ifstream file(filename);
  if (!file.is_open()) throw FileNotFoundException(filename);

  std::string word;
  while (std::getline(file, word)) {
    if (word.empty()) continue;
    Check(word);
  }
}
