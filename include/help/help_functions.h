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

** File help_functions.h: functions that present messages to the user
** (help, usage, and non-exception warnings/errors).
**/

#ifndef HELP_FUNCTIONS_H_
#define HELP_FUNCTIONS_H_

#include <string>

#include "io/options.h"

void Help();
void Usage();
int ValidateArguments(int argc, char* argv[], Options* options);
void PrintWarning(const std::string& message);
void PrintError(const std::string& message);

#endif
