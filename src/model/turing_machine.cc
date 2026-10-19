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

** File turing_machine.cc: the formal definition of a deterministic,
** multi-tape Turing machine.
**/

#include "model/turing_machine.h"

#include <map>

#include "exceptions/exceptions.h"

namespace {

/**
 * @brief A string that uniquely identifies a (state, read) pair, used
 * to detect two transitions that would make the machine
 * non-deterministic.
 */
std::string DeterminismKey(const std::string& state,
                            const std::vector<char>& read) {
  std::string key = state;
  for (char symbol : read) {
    key += '\x1f';
    key += symbol;
  }
  return key;
}

/**
 * @brief Whether two transitions already agree on everything, i.e. one
 * is a harmless exact duplicate of the other rather than a genuine
 * conflict.
 */
bool SameOutcome(const TMTransition& a, const TMTransition& b) {
  return a.to == b.to && a.write == b.write && a.move == b.move;
}

}  // namespace

/**
 * @brief Constructs a Turing machine from its formal definition.
 * @param states The set of states Q.
 * @param input_alphabet The input alphabet Sigma.
 * @param tape_alphabet The tape alphabet Gamma.
 * @param initial_state The initial state s.
 * @param blank_symbol The blank symbol b.
 * @param final_states The set of accepting states F.
 * @param tape_count The number of tapes k.
 * @param transitions The transition function delta.
 * @throws InvalidDefinitionException if the definition is not
 *         well-formed.
 */
TuringMachine::TuringMachine(std::set<std::string> states,
                              Alphabet input_alphabet, Alphabet tape_alphabet,
                              std::string initial_state, char blank_symbol,
                              std::set<std::string> final_states,
                              int tape_count, TMTransitionTable transitions)
    : states_(std::move(states)),
      input_alphabet_(std::move(input_alphabet)),
      tape_alphabet_(std::move(tape_alphabet)),
      initial_state_(std::move(initial_state)),
      blank_symbol_(blank_symbol),
      final_states_(std::move(final_states)),
      tape_count_(tape_count),
      transitions_(std::move(transitions)) {
  Validate();
}

/**
 * @brief Checks the formal well-formedness of the machine definition.
 */
void TuringMachine::Validate() const {
  if (states_.empty()) {
    throw InvalidDefinitionException(0, "Q must not be empty");
  }
  if (states_.find(initial_state_) == states_.end()) {
    throw InvalidDefinitionException(
        0, "initial state '" + initial_state_ + "' is not in Q");
  }
  if (!tape_alphabet_.Contains(blank_symbol_)) {
    throw InvalidDefinitionException(
        0, "blank symbol '" + std::string(1, blank_symbol_) +
               "' is not in Gamma");
  }
  if (input_alphabet_.Contains(blank_symbol_)) {
    throw InvalidDefinitionException(
        0, "blank symbol '" + std::string(1, blank_symbol_) +
               "' cannot belong to Sigma");
  }
  if (!input_alphabet_.IsSubsetOf(tape_alphabet_)) {
    throw InvalidDefinitionException(0, "Sigma must be a subset of Gamma");
  }

  for (const std::string& final_state : final_states_) {
    if (states_.find(final_state) == states_.end()) {
      throw InvalidDefinitionException(
          0, "final state '" + final_state + "' is not in Q");
    }
  }
  if (tape_count_ < 1) {
    throw InvalidDefinitionException(0, "the number of tapes must be at least 1");
  }

  std::map<std::string, TMTransition> seen;
  for (const TMTransition& transition : transitions_.All()) {
    if (states_.find(transition.from) == states_.end()) {
      throw InvalidDefinitionException(
          0, "transition origin state '" + transition.from + "' is not in Q");
    }
    if (states_.find(transition.to) == states_.end()) {
      throw InvalidDefinitionException(
          0, "transition destination state '" + transition.to +
                 "' is not in Q");
    }
    if (static_cast<int>(transition.read.size()) != tape_count_ ||
        static_cast<int>(transition.write.size()) != tape_count_ ||
        static_cast<int>(transition.move.size()) != tape_count_) {
      throw InvalidDefinitionException(
          0, "transition from '" + transition.from +
                 "' does not have one read/write/move entry per tape");
    }
    for (char symbol : transition.read) {
      if (!tape_alphabet_.Contains(symbol)) {
        throw InvalidDefinitionException(
            0, "transition read symbol '" + std::string(1, symbol) +
                   "' is not in Gamma");
      }
    }
    for (char symbol : transition.write) {
      if (!tape_alphabet_.Contains(symbol)) {
        throw InvalidDefinitionException(
            0, "transition write symbol '" + std::string(1, symbol) +
                   "' is not in Gamma");
      }
    }

    std::string key = DeterminismKey(transition.from, transition.read);
    auto existing = seen.find(key);
    if (existing != seen.end() && !SameOutcome(existing->second, transition)) {
      throw InvalidDefinitionException(
          0, "state '" + transition.from +
                 "' has two different transitions for the same symbols "
                 "read, which is not allowed in a deterministic machine");
    }
    seen.emplace(key, transition);
  }
}

/**
 * @brief The set of states Q.
 * @return The set of states Q.
 */
const std::set<std::string>& TuringMachine::States() const { return states_; }

/**
 * @brief The input alphabet Sigma.
 * @return The input alphabet Sigma.
 */
const Alphabet& TuringMachine::InputAlphabet() const { return input_alphabet_; }

/**
 * @brief The tape alphabet Gamma.
 * @return The tape alphabet Gamma.
 */
const Alphabet& TuringMachine::TapeAlphabet() const { return tape_alphabet_; }

/**
 * @brief The initial state s.
 * @return The initial state s.
 */
const std::string& TuringMachine::InitialState() const { return initial_state_; }

/**
 * @brief The blank symbol b.
 * @return The blank symbol b.
 */
char TuringMachine::BlankSymbol() const { return blank_symbol_; }

/**
 * @brief The set of accepting states F.
 * @return The set of accepting states F.
 */
const std::set<std::string>& TuringMachine::FinalStates() const {
  return final_states_;
}

/**
 * @brief The number of tapes k.
 * @return The number of tapes k.
 */
int TuringMachine::TapeCount() const { return tape_count_; }

/**
 * @brief The transition function delta.
 * @return The transition function delta.
 */
const TMTransitionTable& TuringMachine::Transitions() const {
  return transitions_;
}

/**
 * @brief Whether state is one of the machine's accepting (final)
 * states.
 * @param state State to check.
 * @return true if state is in F.
 */
bool TuringMachine::IsAccepting(const std::string& state) const {
  return final_states_.find(state) != final_states_.end();
}
