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

** File simulator.h: step-by-step simulation of a deterministic Turing
** machine.
**/

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <string>

#include "io/trace_writer.h"
#include "model/tm_configuration.h"
#include "model/turing_machine.h"

/**
 * @brief Simulates a TuringMachine on an input word, step by step.
 *
 * Unlike the pushdown automaton project's Simulator, there is no search
 * tree here: the machine is deterministic, so at every step at most one
 * transition applies, and the run is a single straight line from the
 * initial configuration to whichever configuration it halts on (or
 * never halts on). A maximum step count is the only safety net against
 * a machine definition that loops forever on some input, since there
 * is no branching search to bound and no visited-configuration set that
 * could catch a cycle without also risking false positives on tapes
 * that legitimately revisit the same symbols.
 */
class Simulator {
 public:
  explicit Simulator(const TuringMachine& machine, int max_steps = 10000,
                      TraceWriter* trace_writer = nullptr);

  /**
   * @brief Runs the machine on word until it halts.
   * @param word Input word over the machine's input alphabet, placed on
   *        tape 1; every other tape starts out blank.
   * @return The configuration the machine halted on.
   * @throws InvalidWordException if word contains a symbol that is not
   *         in the machine's input alphabet.
   * @throws ExecutionLimitExceededException if the machine does not
   *         halt within the configured maximum number of steps.
   */
  TMConfiguration Run(const std::string& word) const;

 private:
  const TuringMachine& machine_;
  int max_steps_;
  TraceWriter* trace_writer_;
};

#endif
