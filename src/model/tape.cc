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

** File tape.cc: one tape of a Turing machine, infinite in both
** directions.
**/

#include "model/tape.h"

/**
 * @brief Builds a tape with a single blank cell at position 0, with the
 * head on it.
 * @param blank_symbol Symbol every unwritten cell reads as.
 */
Tape::Tape(char blank_symbol)
    : offset_(0), head_(0), min_touched_(0), max_touched_(0), blank_(blank_symbol) {
  cells_.push_back(blank_);
}

/**
 * @brief Extends the backing storage, one blank cell at a time, until
 * logical_position is a valid index into it.
 */
void Tape::EnsureIndex(int logical_position) {
  while (logical_position < offset_) {
    cells_.push_front(blank_);
    --offset_;
  }
  while (logical_position >= offset_ + static_cast<int>(cells_.size())) {
    cells_.push_back(blank_);
  }
}

/**
 * @brief Converts a logical tape position into an index into cells_.
 * logical_position must already be within the current bounds of
 * cells_ (see EnsureIndex).
 */
int Tape::IndexOf(int logical_position) const {
  return logical_position - offset_;
}

/**
 * @brief Records the head's current position as visited, widening the
 * touched range if needed.
 */
void Tape::Touch() {
  if (head_ < min_touched_) min_touched_ = head_;
  if (head_ > max_touched_) max_touched_ = head_;
}

/**
 * @brief Writes content starting at position 0 and resets the head to
 * position 0, the first symbol of content.
 * @param content Symbols to place on the tape, left to right from
 *        position 0; may be empty.
 */
void Tape::LoadInitialContent(const std::string& content) {
  for (size_t i = 0; i < content.size(); ++i) {
    head_ = static_cast<int>(i);
    EnsureIndex(head_);
    cells_[static_cast<size_t>(IndexOf(head_))] = content[i];
    Touch();
  }
  head_ = 0;
  EnsureIndex(head_);
  Touch();
}

/**
 * @brief The symbol currently under the head.
 * @return The symbol currently under the head.
 */
char Tape::Read() const {
  return cells_[static_cast<size_t>(IndexOf(head_))];
}

/**
 * @brief Overwrites the symbol currently under the head.
 * @param symbol Symbol to write.
 */
void Tape::Write(char symbol) {
  cells_[static_cast<size_t>(IndexOf(head_))] = symbol;
}

/**
 * @brief Moves the head one cell left or right, or leaves it in place.
 * @param movement Direction to move the head.
 */
void Tape::Move(Movement movement) {
  switch (movement) {
    case Movement::kLeft:
      --head_;
      break;
    case Movement::kRight:
      ++head_;
      break;
    case Movement::kStay:
      break;
  }
  EnsureIndex(head_);
  Touch();
}

/**
 * @brief The head's current logical position (can be negative).
 * @return The head's current logical position.
 */
int Tape::HeadPosition() const { return head_; }

/**
 * @brief Renders the tape as text, from the leftmost to the rightmost
 * position ever visited or written, with the head's cell in brackets.
 * @return A string representation of the tape, e.g. "ab[c]d".
 */
std::string Tape::ToString() const {
  std::string result;
  for (int pos = min_touched_; pos <= max_touched_; ++pos) {
    char symbol = cells_[static_cast<size_t>(IndexOf(pos))];
    if (pos == head_) {
      result += '[';
      result += symbol;
      result += ']';
    } else {
      result += symbol;
    }
  }
  return result;
}
