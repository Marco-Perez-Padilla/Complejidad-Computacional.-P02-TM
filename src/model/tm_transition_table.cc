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

** File tm_transition_table.cc: storage for a Turing machine's
** transitions, with lookup by (state, symbols read on every tape).
**/

#include "model/tm_transition_table.h"

/**
 * @brief Adds a transition to the table.
 * @param transition Transition to add.
 */
void TMTransitionTable::Add(const TMTransition& transition) {
  transitions_.push_back(transition);
}

/**
 * @brief Looks up the transition applicable from state when read is the
 * tuple of symbols currently under every tape's head.
 * @param state Current state.
 * @param read Symbols read on every tape, in tape order.
 * @return The matching transition, or std::nullopt if none matches.
 */
std::optional<TMTransition> TMTransitionTable::Find(
    const std::string& state, const std::vector<char>& read) const {
  for (const TMTransition& transition : transitions_) {
    if (transition.from == state && transition.read == read) {
      return transition;
    }
  }
  return std::nullopt;
}

/**
 * @brief Number of transitions stored in the table.
 * @return Number of transitions stored in the table.
 */
size_t TMTransitionTable::Size() const { return transitions_.size(); }
