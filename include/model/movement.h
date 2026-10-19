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

** File movement.h: the three ways a tape's read/write head can move.
**/

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

/**
 * @brief The three ways the read/write head of a tape can move after a
 * transition is applied: one cell left, one cell right, or stay in
 * place.
 *
 * Reading, writing and moving all happen together as a single,
 * indivisible transition (see TMTransition), so kStay is a real,
 * meaningful outcome here, not just "no movement field given" .
 */
enum class Movement {
  kLeft,
  kRight,
  kStay,
};

#endif
