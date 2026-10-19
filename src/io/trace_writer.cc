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

** File trace_writer.cc: formats the simulator's run as a readable
** trace.
**/

#include "io/trace_writer.h"

namespace {

/**
 * @brief Formats a movement for printing as 'L', 'R' or 'S'.
 */
char FormatMovement(Movement movement) {
  switch (movement) {
    case Movement::kLeft:
      return 'L';
    case Movement::kRight:
      return 'R';
    case Movement::kStay:
      return 'S';
  }
  return '?';
}

}  // namespace

/**
 * @brief Constructs a TraceWriter instance.
 * @param out The output stream to write the trace to.
 */
TraceWriter::TraceWriter(std::ostream& out) : out_(out) {}

/**
 * @brief Returns a string of spaces for indentation based on the step
 * number.
 * @param step The current step number.
 * @return A string of spaces for indentation.
 */
std::string TraceWriter::Indent(int step) const {
  return std::string(static_cast<size_t>(step) * 2, ' ');
}

/**
  * @brief Prints the header shown once per simulated word.
*/
void TraceWriter::PrintHeader(const std::string& word) {
  out_ << "Word: " << (word.empty() ? "eps" : word) << "\n";
}

/**
  * @brief Prints one configuration of the run: the current step, state
  * and the content of every tape.
*/
void TraceWriter::PrintConfiguration(int step, const TMConfiguration& configuration) {
  out_ << Indent(step) << "(" << configuration.state;
  for (size_t i = 0; i < configuration.tapes.size(); ++i) {
    out_ << ", tape" << i + 1 << "=" << configuration.tapes[i].ToString();
  }
  out_ << ")\n";
}

/**
  * @brief Prints the transition being applied to move to the next step.
*/
void TraceWriter::PrintTransition(int step, const TMTransition& transition) {
  out_ << Indent(step) << "-> (" << transition.from << ", read ";
  for (size_t i = 0; i < transition.read.size(); ++i) {
    if (i > 0) out_ << " ";
    out_ << transition.read[i];
  }
  out_ << ") => (" << transition.to << ", write ";
  for (size_t i = 0; i < transition.write.size(); ++i) {
    if (i > 0) out_ << " ";
    out_ << transition.write[i];
  }
  out_ << ", move ";
  for (size_t i = 0; i < transition.move.size(); ++i) {
    if (i > 0) out_ << " ";
    out_ << FormatMovement(transition.move[i]);
  }
  out_ << ")\n";
}

/**
  * @brief Prints that the machine halted at this step because no
  * transition applies.
*/
void TraceWriter::PrintHalted(int step) {
  out_ << Indent(step) << "no applicable transition; machine halts\n";
}

/**
  * @brief Prints the final verdict and the resulting first tape for the
  * simulated word.
*/
void TraceWriter::PrintResult(bool accepted, const std::string& final_tape) {
  out_ << (accepted ? "Result: ACCEPTED" : "Result: REJECTED")
       << "  tape 1: " << final_tape << "\n\n";
}
