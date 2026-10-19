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

** File simulator.cc: implementation of Simulator.
**/

#include "simulation/simulator.h"

#include <vector>

#include "exceptions/exceptions.h"

/**
  * @brief Builds a simulator for the given machine.
  * @param machine Machine to simulate; must outlive the Simulator.
  * @param max_steps Maximum number of steps a single run may take
  *        before it is considered a probable infinite loop.
  * @param trace_writer If not null, every step is reported to it as the
  *        run progresses.
*/
Simulator::Simulator(const TuringMachine& machine, int max_steps,
                      TraceWriter* trace_writer)
    : machine_(machine), max_steps_(max_steps), trace_writer_(trace_writer) {}

/**
  * @brief Runs the machine on word until it halts.
  * @param word Input word over the machine's input alphabet.
  * @return The configuration the machine halted on.
  * @throws InvalidWordException if word contains a symbol that is not
  *         in the machine's input alphabet.
  * @throws ExecutionLimitExceededException if the machine does not halt
  *         within the configured maximum number of steps.
*/
TMConfiguration Simulator::Run(const std::string& word) const {
  for (char symbol : word) {
    if (!machine_.InputAlphabet().Contains(symbol)) {
      throw InvalidWordException(word, symbol);
    }
  }

  if (trace_writer_ != nullptr) {
    trace_writer_->PrintHeader(word);
  }

  TMConfiguration configuration;
  configuration.state = machine_.InitialState();
  for (int i = 0; i < machine_.TapeCount(); ++i) {
    configuration.tapes.emplace_back(machine_.BlankSymbol());
  }
  configuration.tapes[0].LoadInitialContent(word);

  bool halted = false;
  int step = 0;
  for (; step < max_steps_; ++step) {
    if (trace_writer_ != nullptr) {
      trace_writer_->PrintConfiguration(step, configuration);
    }

    std::vector<char> read;
    read.reserve(configuration.tapes.size());
    for (const Tape& tape : configuration.tapes) read.push_back(tape.Read());

    std::optional<TMTransition> transition =
        machine_.Transitions().Find(configuration.state, read);
    if (!transition.has_value()) {
      if (trace_writer_ != nullptr) trace_writer_->PrintHalted(step);
      halted = true;
      break;
    }

    if (trace_writer_ != nullptr) {
      trace_writer_->PrintTransition(step, *transition);
    }

    configuration.state = transition->to;
    for (size_t i = 0; i < configuration.tapes.size(); ++i) {
      configuration.tapes[i].Write(transition->write[i]);
      configuration.tapes[i].Move(transition->move[i]);
    }
  }

  if (!halted) throw ExecutionLimitExceededException(word);

  bool accepted = machine_.IsAccepting(configuration.state);
  if (trace_writer_ != nullptr) {
    trace_writer_->PrintResult(accepted, configuration.tapes[0].ToString());
  }
  return configuration;
}
