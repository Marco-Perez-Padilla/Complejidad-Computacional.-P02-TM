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

** File tm_configuration.h: an instantaneous description of a Turing
** machine during simulation.
**/

#ifndef TM_CONFIGURATION_H_
#define TM_CONFIGURATION_H_

#include <string>
#include <vector>

#include "model/tape.h"

/**
 * @brief The state of a Turing machine at one point of the simulation:
 * its current state and the full content and head position of every
 * tape.
 *
 * Unlike a pushdown automaton's Configuration, this has no Key(): the
 * machine is deterministic, so the simulation never branches and does
 * not need to detect already-visited configurations to avoid repeating
 * work. An upper bound on the number of steps (see Simulator) is what
 * guards against a machine that loops forever instead.
 */
struct TMConfiguration {
  std::string state;
  std::vector<Tape> tapes;
};

#endif
