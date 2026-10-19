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

** File alphabet.cc: set of single-character symbols, used for both the
** input alphabet (Sigma) and the tape alphabet (Gamma) of a Turing
** machine.
**/

#include "model/alphabet.h"

/**
  * @brief Checks whether a character cannot belong to any alphabet
  * because it is reserved by the file format.
  * @param symbol Character to check.
  * @return true if symbol is '#' (comment marker).
*/
bool Alphabet::IsReserved(char symbol) {
  return symbol == '#';
}

/**
  * @brief Adds a symbol to the alphabet, unless it is reserved.
  * @param symbol Character to add.
  * @return true if the symbol was added (or already present); false if
  *         symbol is reserved and therefore was not added.
*/
bool Alphabet::AddSymbol(char symbol) {
  if (IsReserved(symbol)) return false;
  symbols_.insert(symbol);
  return true;
}

/**
  * @brief Checks whether a symbol belongs to the alphabet.
  * @param symbol Character to check.
  * @return true if the symbol is in the alphabet; false otherwise.
*/
bool Alphabet::Contains(char symbol) const {
  return symbols_.find(symbol) != symbols_.end();
}

/**
  * @brief Number of distinct symbols in the alphabet.
  * @return Number of symbols.
*/
size_t Alphabet::Size() const { return symbols_.size(); }

/**
 * @brief Checks whether the alphabet has no symbols.
 * @return true if the alphabet is empty; false otherwise.
 */
bool Alphabet::Empty() const { return symbols_.empty(); }

/**
  * @brief Checks whether every symbol of this alphabet also belongs to
  * other.
  * @param other Alphabet to check membership against.
  * @return true if this alphabet is a subset of other (an empty
  *         alphabet is a subset of any alphabet).
*/
bool Alphabet::IsSubsetOf(const Alphabet& other) const {
  for (char symbol : symbols_) {
    if (!other.Contains(symbol)) return false;
  }
  return true;
}
