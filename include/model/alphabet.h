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

** File alphabet.h: set of single-character symbols, used for both the
** input alphabet (Sigma) and the tape alphabet (Gamma) of a Turing
** machine.
**/

#ifndef ALPHABET_H_
#define ALPHABET_H_

#include <set>

/**
 * @brief Represents a finite set of single-character symbols.
 *
 * Used for both Sigma and Gamma. Centralizes membership checks and
 * rejects the character reserved by the definition file format ('#' for
 * comments). Unlike a pushdown automaton's format, '.' is not reserved
 * here: it is commonly used as the Turing machine's own blank symbol, so
 * it must be free to belong to Gamma.
 */
class Alphabet {
 public:
  Alphabet() = default;

  static bool IsReserved(char symbol);
  bool AddSymbol(char symbol);
  bool Contains(char symbol) const;
  size_t Size() const;
  bool Empty() const;

  /**
   * @brief Checks whether every symbol of this alphabet also belongs to
   * other, e.g. to verify that Sigma is a subset of Gamma.
   */
  bool IsSubsetOf(const Alphabet& other) const;

 private:
  std::set<char> symbols_;
};

#endif
