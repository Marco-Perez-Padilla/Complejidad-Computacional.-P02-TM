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

** File turing_machine.h: the formal definition of a deterministic,
** multi-tape Turing machine.
**/

#ifndef TURING_MACHINE_H_
#define TURING_MACHINE_H_

#include <set>
#include <string>

#include "model/alphabet.h"
#include "model/tm_transition_table.h"

/**
 * @brief The formal definition of a deterministic Turing machine with
 * one or more tapes: M = (Q, Sigma, Gamma, s, b, F, delta), plus the
 * number of tapes k.
 *
 * The constructor validates the definition against the formal
 * requirements of a Turing machine (initial state in Q, blank symbol in
 * Gamma but not in Sigma, Sigma subset of Gamma, final states subset of
 * Q, at least one tape, every transition referring only to declared
 * states and symbols with exactly k entries per tape-indexed field, and
 * no two transitions giving conflicting outcomes for the same (state,
 * read) pair) and throws InvalidDefinitionException if any of them is
 * violated.
 *
 * There is only one acceptance mode here (unlike the pushdown
 * automaton project, which had to support two): the machine accepts a
 * word if, once it halts for lack of an applicable transition, its
 * current state is one of the final states in F. A Strategy/Factory
 * pair for acceptance would just be ceremony with nothing to select
 * between, so IsAccepting is a plain membership check instead.
 */
class TuringMachine {
 public:
  TuringMachine(std::set<std::string> states, Alphabet input_alphabet,
                Alphabet tape_alphabet, std::string initial_state,
                char blank_symbol, std::set<std::string> final_states,
                int tape_count, TMTransitionTable transitions);

  const std::set<std::string>& States() const;
  const Alphabet& InputAlphabet() const;
  const Alphabet& TapeAlphabet() const;
  const std::string& InitialState() const;
  char BlankSymbol() const;
  const std::set<std::string>& FinalStates() const;
  int TapeCount() const;
  const TMTransitionTable& Transitions() const;

  /**
   * @brief Whether state is one of the machine's accepting (final)
   * states.
   */
  bool IsAccepting(const std::string& state) const;

 private:
  void Validate() const;

  std::set<std::string> states_;
  Alphabet input_alphabet_;
  Alphabet tape_alphabet_;
  std::string initial_state_;
  char blank_symbol_;
  std::set<std::string> final_states_;
  int tape_count_;
  TMTransitionTable transitions_;
};

#endif
