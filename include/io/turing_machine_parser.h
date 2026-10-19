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

** File turing_machine_parser.h: reads a Turing machine definition from
** a text file.
**/

#ifndef TURING_MACHINE_PARSER_H_
#define TURING_MACHINE_PARSER_H_

#include <string>

#include "model/turing_machine.h"

/**
 * @brief Reads a Turing machine definition from filename and builds the
 * corresponding TuringMachine.
 *
 * The file has one element per line, in this order: Q, Sigma, Gamma,
 * the initial state, the blank symbol, F, the number of tapes k, and
 * then one transition per line (each with 2 + 3k whitespace-separated
 * fields: the origin state, k symbols read, the destination state, k
 * symbols written and k moves in {L, R, S}). Comments start with '#'.
 *
 * @param filename Path to the definition file.
 * @return The parsed, validated machine.
 * @throws FileNotFoundException if filename cannot be opened.
 * @throws EmptyFileException if the file has no meaningful content.
 * @throws InvalidDefinitionException if the definition is malformed or
 *         violates the formal definition of a Turing machine.
 */
TuringMachine ParseTuringMachineFile(const std::string& filename);

#endif
