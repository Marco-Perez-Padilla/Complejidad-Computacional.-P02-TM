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

** File tm_transition.h: representation of a Turing machine transition.
**/

#ifndef TM_TRANSITION_H_
#define TM_TRANSITION_H_

#include <string>
#include <vector>

#include "model/movement.h"

/**
 * @brief A Turing machine transition for a machine with k tapes:
 * delta(from, read[0..k-1]) = (to, write[0..k-1], move[0..k-1]).
 *
 * Reading, writing and moving happen together as a single, indivisible
 * step: every tape's symbol under the head is replaced by the matching
 * entry of write, and its head then moves according to the matching
 * entry of move. read, write and move always have exactly as many
 * entries as the machine has tapes.
 */
struct TMTransition {
  std::string from;
  std::vector<char> read;
  std::string to;
  std::vector<char> write;
  std::vector<Movement> move;
};

#endif
