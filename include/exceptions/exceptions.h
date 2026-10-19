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

** File exceptions.h: exception hierarchy for the Turing machine
** simulator.
**/

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>
#include <string>

/**
 * @brief Base class for every exception thrown by the simulator.
 */
class TmException : public std::exception {
 public:
  explicit TmException(const std::string& message) : message_(message) {}

  const char* what() const noexcept override { return message_.c_str(); }

 private:
  std::string message_;
};

/**
 * @brief Thrown when a file requested by the user cannot be opened
 * (machine definition file or input strings file).
 */
class FileNotFoundException : public TmException {
 public:
  explicit FileNotFoundException(const std::string& filename)
      : TmException("cannot open file '" + filename + "'") {}
};

/**
 * @brief Thrown when a file exists but is empty or has no valid
 * content.
 */
class EmptyFileException : public TmException {
 public:
  explicit EmptyFileException(const std::string& filename)
      : TmException("file '" + filename + "' is empty or has no valid content") {}
};

/**
 * @brief Thrown when the command-line arguments are incorrect or
 * incomplete.
 */
class InvalidArgumentsException : public TmException {
 public:
  explicit InvalidArgumentsException(const std::string& reason)
      : TmException("invalid arguments: " + reason) {}
};

/**
 * @brief Thrown when the machine definition violates the formal
 * definition of a Turing machine (e.g. the initial state is not in Q).
 */
class InvalidDefinitionException : public TmException {
 public:
  InvalidDefinitionException(int line, const std::string& reason)
      : TmException("line " + std::to_string(line) + ": invalid definition: " + reason) {}
};

/**
 * @brief Thrown when an input word contains a symbol that does not
 * belong to the machine's input alphabet (Sigma).
 */
class InvalidWordException : public TmException {
 public:
  InvalidWordException(const std::string& word, char symbol)
      : TmException("word '" + word + "' contains symbol '" + std::string(1, symbol) + "' which is not in Sigma") {}
};

/**
 * @brief Thrown when simulating a word takes more steps than the
 * configured safety limit without halting. Since the machine is
 * deterministic, this is the only way to guard against a definition
 * that loops forever on some input: there is no search tree to bound,
 * only a single, possibly endless, computation.
 */
class ExecutionLimitExceededException : public TmException {
 public:
  explicit ExecutionLimitExceededException(const std::string& word)
      : TmException("execution on word '" + word +
                     "' exceeded the maximum number of steps (possible infinite loop)") {}
};

#endif
