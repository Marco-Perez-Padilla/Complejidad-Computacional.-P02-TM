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

** File mt.cc: Client program for simulating a Turing machine. It
** validates the command-line arguments, reads the machine definition
** and input words, and reports whether each word is accepted.
**/

#include "exceptions/exceptions.h"
#include "help/help_functions.h"
#include "io/application.h"
#include "io/options.h"

int main(int argc, char* argv[]) {
  Options options;
  int status = ValidateArguments(argc, argv, &options);
  if (status != -1) return status;

  try {
    Application(options).Run();
  } catch (const TmException& error) {
    PrintError(error.what());
    return 1;
  }
  return 0;
}
