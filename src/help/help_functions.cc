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

** File help_functions.cc: functions that present messages to the user
** (help, usage, and non-exception warnings/errors).
**/

#include "help/help_functions.h"

#include <iostream>

/**
 * @brief Prints a short summary of the program and its options.
 */
void Help() {
  std::cout
      << "Turing machine simulator\n\n"
      << "Usage: ./mt -config <file> [-trace] [-in <file>] [-out <file>]\n\n"
      << "Options:\n"
      << "  -config <file>   Machine definition file (required)\n"
      << "  -trace           Show the step-by-step simulation trace\n"
      << "  -in <file>       Input strings to check (optional; keyboard "
         "by default)\n"
      << "  -out <file>      File to write the trace to (optional; "
         "screen by default)\n"
      << "  --help, -h       Show this message and exit\n";
}

/**
 * @brief Prints a detailed description of the program, its command-line
 * options and the machine definition file format. Called when the
 * arguments provided are incorrect.
 */
void Usage() {
  std::cout
      << "Turing machine (TM) simulator\n\n"
      << "Simulates a deterministic, multi-tape Turing machine: a word "
         "is\n"
      << "accepted when the machine halts (runs out of applicable "
         "transitions)\n"
      << "in one of its final states.\n\n"
      << "Command-line options:\n"
      << "  -config <file>   Machine definition file (required)\n"
      << "  -trace           Enable the simulation trace, showing the "
         "state,\n"
      << "                    every tape and the transition applied "
         "after\n"
      << "                    every step\n"
      << "  -in <file>       Input strings to check, one per line "
         "(optional;\n"
      << "                    without it, strings are read from the "
         "keyboard)\n"
      << "  -out <file>      File to write the trace to (optional; "
         "without it,\n"
      << "                    the trace is printed on screen)\n\n"
      << "Definition file format (comments start with '#'):\n"
      << "  q1 q2 q3 ...      set of states Q\n"
      << "  a1 a2 a3 ...      input alphabet Sigma\n"
      << "  A1 A2 A3 ...      tape alphabet Gamma\n"
      << "  q1                initial state\n"
      << "  b                 blank symbol (must be in Gamma, not in "
         "Sigma)\n"
      << "  q2 q3 ...         set of final (accepting) states F\n"
      << "  k                 number of tapes\n"
      << "  q1 r1..rk q2 w1..wk m1..mk\n"
      << "                    one transition per line: delta(q1, "
         "r1..rk) =\n"
      << "                    (q2, w1..wk, m1..mk), with one read/write/"
         "move\n"
      << "                    symbol per tape and each m in {L, R, S}\n\n"
      << "On keyboard input, '.' denotes the empty string and '..' "
         "terminates\n"
      << "the program.\n\n"
      << "Example definition files are in data/definitions/\n";
}

/**
 * @brief Prints a non-critical warning to stderr, with a uniform
 * prefix. A warning does not stop program execution.
 * @param message Description of the warning.
 */
void PrintWarning(const std::string& message) {
  std::cerr << "Warning: " << message << '\n';
}

/**
 * @brief Prints a critical error to stderr, with a uniform prefix. The
 * caller decides whether the program should terminate.
 * @param message Description of the error.
 */
void PrintError(const std::string& message) {
  std::cerr << "Error: " << message << '\n';
}

/**
 * @brief Parses and validates argv into options.
 * @param argc Argument count, as received by main.
 * @param argv Argument vector, as received by main.
 * @param options Filled in with the parsed options on success.
 * @return 0 if --help/-h was requested (Help() was already printed);
 *         -1 if the arguments are correct and options is ready to use;
 *          1 if the arguments are incorrect (an error and Usage() were
 *         already printed).
 */
int ValidateArguments(int argc, char* argv[], Options* options) {
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      Help();
      return 0;
    }
  }

  bool has_config = false;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-config") {
      if (i + 1 >= argc) {
        PrintError("-config requires a file name");
        Usage();
        return 1;
      }
      options->config_file = argv[++i];
      has_config = true;
    } else if (arg == "-trace") {
      options->trace = true;
    } else if (arg == "-in") {
      if (i + 1 >= argc) {
        PrintError("-in requires a file name");
        Usage();
        return 1;
      }
      options->input_file = argv[++i];
    } else if (arg == "-out") {
      if (i + 1 >= argc) {
        PrintError("-out requires a file name");
        Usage();
        return 1;
      }
      options->output_file = argv[++i];
    } else {
      PrintError("unknown option '" + arg + "'");
      Usage();
      return 1;
    }
  }

  if (!has_config) {
    PrintError("-config <file> is required");
    Usage();
    return 1;
  }
  if (options->output_file.has_value() && !options->trace) {
    PrintWarning("-out has no effect without -trace; ignoring it");
    options->output_file.reset();
  }
  return -1;
}
