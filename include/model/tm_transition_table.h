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

** File tm_transition_table.h: storage for a Turing machine's
** transitions, with lookup by (state, symbols read on every tape).
**/

#ifndef TM_TRANSITION_TABLE_H_
#define TM_TRANSITION_TABLE_H_

#include <optional>
#include <string>
#include <vector>

#include "model/tm_transition.h"

/**
 * @brief Stores a Turing machine's transition function and answers
 * lookups by (state, symbols read on every tape).
 *
 * The machine is deterministic, so at most one transition can match a
 * given (state, read) pair; Find returns it if one is present. This
 * class is a plain container: whether two added transitions actually
 * conflict (same state and read, different outcome) is checked by the
 * caller (the parser and TuringMachine::Validate), not here.
 */
class TMTransitionTable {
 public:
  TMTransitionTable() = default;

  void Add(const TMTransition& transition);
  const std::vector<TMTransition>& All() const { return transitions_; }
  std::optional<TMTransition> Find(const std::string& state,
                                    const std::vector<char>& read) const;
  size_t Size() const;

 private:
  std::vector<TMTransition> transitions_;
};

#endif
