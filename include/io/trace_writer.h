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

** File trace_writer.h: formats the simulator's run as a readable trace.
**/

#ifndef TRACE_WRITER_H_
#define TRACE_WRITER_H_

#include <optional>
#include <ostream>
#include <string>

#include "model/tm_configuration.h"
#include "model/tm_transition.h"

/**
 * @brief Formats the step-by-step run performed by Simulator as a
 * readable trace written to an output stream.
 *
 * Kept separate from Simulator so that changing how the trace looks
 * never requires touching the simulation logic itself, the same way
 * the pushdown automaton project's TraceWriter is kept separate from
 * its Simulator.
 */
class TraceWriter {
 public:
  explicit TraceWriter(std::ostream& out);

  void PrintHeader(const std::string& word);
  void PrintConfiguration(int step, const TMConfiguration& configuration);
  void PrintTransition(int step, const TMTransition& transition);
  void PrintHalted(int step);
  void PrintResult(bool accepted, const std::string& final_tape);

 private:
  std::string Indent(int step) const;

  std::ostream& out_;
};

#endif
