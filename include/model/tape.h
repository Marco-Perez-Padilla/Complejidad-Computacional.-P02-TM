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

** File tape.h: one tape of a Turing machine, infinite in both
** directions.
**/

#ifndef TAPE_H_
#define TAPE_H_

#include <deque>
#include <string>

#include "model/movement.h"

/**
 * @brief One tape of a Turing machine: a sequence of cells, infinite in
 * both directions, together with the position of its read/write head.
 *
 * Unwritten cells read as the blank symbol. The backing storage only
 * grows one cell at a time, in whichever direction the head moves past
 * its current bounds, instead of being allocated up front, so a
 * machine that only ever visits a few cells never pays for an
 * actually-infinite tape. Logical positions (Tape::HeadPosition, the
 * positions used internally) can be negative: position 0 is simply
 * where the head starts, not a left boundary.
 */
class Tape {
 public:
  explicit Tape(char blank_symbol);

  /**
   * @brief Writes content onto the tape starting at logical position 0,
   * one symbol per cell, and leaves the head at position 0 (the first
   * symbol of content), as required for the input tape of a
   * simulation. content may be empty, leaving the tape all blank.
   */
  void LoadInitialContent(const std::string& content);

  /**
   * @brief The symbol currently under the head.
   */
  char Read() const;

  /**
   * @brief Overwrites the symbol currently under the head.
   */
  void Write(char symbol);

  /**
   * @brief Moves the head one cell left or right, or leaves it in
   * place, extending the tape with blank cells if the head moves past
   * its current bounds.
   */
  void Move(Movement movement);

  /**
   * @brief The head's current logical position (can be negative).
   */
  int HeadPosition() const;

  /**
   * @brief Renders the tape as text: every cell from the leftmost to
   * the rightmost position ever visited or written, with the head's
   * cell marked by brackets, e.g. "ab[c]d". A tape nothing has touched
   * beyond its starting cell is rendered as "[<blank>]".
   */
  std::string ToString() const;

 private:
  void EnsureIndex(int logical_position);
  int IndexOf(int logical_position) const;
  void Touch();

  std::deque<char> cells_;
  int offset_;        // Logical position represented by cells_[0].
  int head_;           // Logical position of the head; can be negative.
  int min_touched_;    // Leftmost position ever visited or written.
  int max_touched_;    // Rightmost position ever visited or written.
  char blank_;
};

#endif
