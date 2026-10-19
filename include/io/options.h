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

** File options.h: command-line options accepted by the simulator.
**/

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <optional>
#include <string>

/**
 * @brief Command-line options accepted by the simulator, filled in by
 * ValidateArguments (see help/help_functions.h).
 */
struct Options {
  std::string config_file;
  bool trace = false;
  std::optional<std::string> input_file;
  std::optional<std::string> output_file;
};

#endif
