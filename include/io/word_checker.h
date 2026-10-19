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

** File word_checker.h: checks whether words are accepted by a
** TuringMachine, optionally tracing the simulation.
**/

#ifndef WORD_CHECKER_H_
#define WORD_CHECKER_H_

#include <ostream>
#include <string>

#include "model/turing_machine.h"

/**
 * @brief Checks words against a fixed machine, printing one
 * ACCEPTED/REJECTED result per word together with the resulting tape.
 * A word outside Sigma is reported as a warning, not as a crash; so is
 * a word whose simulation exceeds the step limit.
 *
 * Bundles the machine together with the trace flag and the streams
 * every check needs, so callers (Menu, file batches) don't have to keep
 * re-passing the same arguments on every word — only the word itself
 * varies between calls.
 */
class WordChecker {
 public:
  /**
   * @param machine Machine words are checked against.
   * @param trace Whether to write the simulation trace for every word.
   * @param out Stream the ACCEPTED/REJECTED results are written to.
   * @param trace_out Stream the trace is written to when trace is true.
   */
  WordChecker(const TuringMachine& machine, bool trace, std::ostream& out,
              std::ostream& trace_out);

  /**
   * @brief Checks a single word and prints the result to out. '.' is
   * treated as the empty word, matching the file/keyboard input format.
   */
  void Check(const std::string& word) const;

  /**
   * @brief Checks every non-empty line of filename, printing one
   * ACCEPTED/REJECTED result per line to out. A line whose word
   * contains a symbol outside the machine's input alphabet, or whose
   * simulation exceeds the step limit, is reported as a warning and
   * skipped, without stopping the rest of the file.
   * @param filename Path to the file with one word per line.
   * @throws FileNotFoundException if filename cannot be opened.
   */
  void CheckAllFromFile(const std::string& filename) const;

 private:
  const TuringMachine& machine_;
  bool trace_;
  std::ostream& out_;
  std::ostream& trace_out_;
};

#endif
