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

** File application.h: top-level orchestration invoked by main after
** the command-line arguments are validated.
**/

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <fstream>
#include <iostream>

#include "io/options.h"
#include "model/turing_machine.h"

/**
 * @brief Loads the machine described by an Options, then checks words
 * against it: from options.input_file if given, otherwise
 * interactively through a Menu. Writes the trace to options.output_file
 * when requested, or to standard output otherwise.
 *
 * Bundles the options together with the streams used for input/output,
 * the same way Menu already bundles its own streams, so the whole run
 * can be exercised in tests with string streams instead of
 * std::cin/std::cout.
 */
class Application {
 public:
  /**
   * @brief Builds an application bound to already-validated options and
   * to the streams used for interaction. in/out default to
   * std::cin/std::cout so production code doesn't have to pass them
   * explicitly.
   */
  explicit Application(Options options, std::istream& in = std::cin,
                        std::ostream& out = std::cout);

  /**
   * @brief Loads the machine, opens the trace output if requested, and
   * checks words against it: from options.input_file if given,
   * otherwise interactively through a Menu.
   * @throws TmException (or a subclass) if the machine definition, the
   *         input file or the output file cannot be processed.
   */
  void Run();

 private:
  // Parses options_.config_file and reports a one-line summary to out_.
  TuringMachine LoadMachine() const;

  // Opens trace_file on options_.output_file when tracing to a file was
  // requested and returns a reference to whichever stream the trace
  // should go to. trace_file must outlive the returned reference, so it
  // is owned by the caller (Run), not by this method.
  std::ostream& SelectTraceStream(std::ofstream& trace_file) const;

  Options options_;
  std::istream& in_;
  std::ostream& out_;
};

#endif
