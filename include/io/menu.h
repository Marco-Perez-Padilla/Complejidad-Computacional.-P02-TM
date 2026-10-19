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

** File menu.h: interactive loop that lets the user check words against
** a WordChecker, from the keyboard or from a file.
**/

#ifndef MENU_H_
#define MENU_H_

#include <istream>
#include <ostream>
#include <string>

#include "io/word_checker.h"

/**
 * @brief Interactive loop that lets the user check whether words are
 * accepted by a machine, entering them either from the keyboard or from
 * a file, until they choose to exit.
 */
class Menu {
 public:
  /**
   * @brief Builds a menu bound to a WordChecker and to the streams used
   * for interaction, so it can be exercised in tests with string
   * streams instead of std::cin/std::cout.
   */
  Menu(const WordChecker& checker, std::istream& in, std::ostream& out);

  /**
   * @brief Runs the interactive loop until the user chooses to exit or
   * the input stream ends.
   */
  void Run();

 private:
  void RunKeyboardMode();
  void RunFileMode();

  const WordChecker& checker_;
  std::istream& in_;
  std::ostream& out_;
};

#endif
