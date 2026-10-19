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

** File turing_machine_parser.cc: reads a Turing machine definition from
** a text file.
**/

#include "io/turing_machine_parser.h"

#include <fstream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "exceptions/exceptions.h"
#include "help/help_functions.h"
#include "model/movement.h"

namespace {

struct RawLine {
  int number;
  std::string content;
};

/**
 * @brief Removes a trailing '#' comment from line and trims the
 * surrounding whitespace.
 */
std::string StripComment(const std::string& line) {
  std::string result = line.substr(0, line.find('#'));
  size_t begin = result.find_first_not_of(" \t\r");
  if (begin == std::string::npos) return "";
  size_t end = result.find_last_not_of(" \t\r");
  return result.substr(begin, end - begin + 1);
}

/**
 * @brief Splits line into whitespace-separated tokens.
 */
std::vector<std::string> Tokenize(const std::string& line) {
  std::istringstream stream(line);
  std::vector<std::string> tokens;
  std::string token;
  while (stream >> token) tokens.push_back(token);
  return tokens;
}

/**
 * @brief Reads filename and returns every line with content, after
 * stripping comments, together with its original line number.
 */
std::vector<RawLine> ReadMeaningfulLines(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) throw FileNotFoundException(filename);

  std::vector<RawLine> lines;
  std::string raw;
  int number = 0;
  while (std::getline(file, raw)) {
    ++number;
    std::string content = StripComment(raw);
    if (!content.empty()) lines.push_back({number, content});
  }
  if (lines.empty()) throw EmptyFileException(filename);
  return lines;
}

/**
 * @brief Parses the tokens of line as an Alphabet (Sigma or Gamma).
 */
Alphabet ParseAlphabet(const RawLine& line, const std::string& alphabet_name) {
  Alphabet alphabet;
  for (const std::string& token : Tokenize(line.content)) {
    if (token.size() != 1) {
      throw InvalidDefinitionException(
          line.number, alphabet_name +
                           " symbols must be a single character: '" +
                           token + "'");
    }
    if (!alphabet.AddSymbol(token[0])) {
      throw InvalidDefinitionException(
          line.number, "'" + token + "' is reserved and cannot belong to " +
                           alphabet_name);
    }
  }
  return alphabet;
}

/**
 * @brief Parses the tokens of line as a set of state names.
 */
std::set<std::string> ParseStateSet(const RawLine& line) {
  std::set<std::string> states;
  for (const std::string& token : Tokenize(line.content)) states.insert(token);
  return states;
}

/**
 * @brief Parses the tokens of line as the set of final states F, every
 * one of which must already be a declared state.
 */
std::set<std::string> ParseFinalStates(const RawLine& line,
                                        const std::set<std::string>& states) {
  std::set<std::string> final_states;
  for (const std::string& token : Tokenize(line.content)) {
    if (states.find(token) == states.end()) {
      throw InvalidDefinitionException(
          line.number, "'" + token + "' is not in Q (expected the set of "
                                      "final states here)");
    }
    final_states.insert(token);
  }
  return final_states;
}

/**
 * @brief Parses line as a single character: the initial state's token
 * is handled separately since states may be multi-character, but the
 * blank symbol must be exactly one character, like every other symbol.
 */
char ParseSingleSymbol(const RawLine& line, const std::string& field_name) {
  std::vector<std::string> tokens = Tokenize(line.content);
  if (tokens.size() != 1 || tokens[0].size() != 1) {
    throw InvalidDefinitionException(
        line.number, "the " + field_name + " must be a single character");
  }
  return tokens[0][0];
}

/**
 * @brief Parses line as the number of tapes k: a single token that must
 * be a positive integer.
 */
int ParseTapeCount(const RawLine& line) {
  std::vector<std::string> tokens = Tokenize(line.content);
  if (tokens.size() != 1) {
    throw InvalidDefinitionException(
        line.number, "the number of tapes must be a single integer");
  }
  try {
    size_t consumed = 0;
    int count = std::stoi(tokens[0], &consumed);
    if (consumed != tokens[0].size() || count < 1) {
      throw InvalidDefinitionException(
          line.number, "the number of tapes must be a positive integer");
    }
    return count;
  } catch (const std::invalid_argument&) {
    throw InvalidDefinitionException(
        line.number, "the number of tapes must be a positive integer");
  } catch (const std::out_of_range&) {
    throw InvalidDefinitionException(line.number, "the number of tapes is too large");
  }
}

/**
 * @brief Parses a single-character movement token as a Movement.
 */
Movement ParseMovement(const RawLine& line, const std::string& token) {
  if (token == "L") return Movement::kLeft;
  if (token == "R") return Movement::kRight;
  if (token == "S") return Movement::kStay;
  throw InvalidDefinitionException(
      line.number, "'" + token + "' is not a valid movement (expected 'L', "
                                  "'R' or 'S')");
}

/**
 * @brief Parses line as a single transition for a machine with
 * tape_count tapes: 2 + 3*tape_count whitespace-separated fields.
 */
TMTransition ParseTransition(const RawLine& line,
                              const std::set<std::string>& states,
                              const Alphabet& tape_alphabet, int tape_count) {
  std::vector<std::string> tokens = Tokenize(line.content);
  size_t expected = static_cast<size_t>(2 + 3 * tape_count);
  if (tokens.size() != expected) {
    throw InvalidDefinitionException(
        line.number, "a transition for a machine with " +
                         std::to_string(tape_count) + " tape(s) must have "
                         "exactly " + std::to_string(expected) + " fields");
  }

  TMTransition transition;
  size_t index = 0;

  transition.from = tokens[index++];
  if (states.find(transition.from) == states.end()) {
    throw InvalidDefinitionException(
        line.number, "state '" + transition.from + "' is not in Q");
  }

  for (int i = 0; i < tape_count; ++i) {
    const std::string& token = tokens[index++];
    if (token.size() != 1 || !tape_alphabet.Contains(token[0])) {
      throw InvalidDefinitionException(
          line.number, "'" + token + "' is not in Gamma");
    }
    transition.read.push_back(token[0]);
  }

  transition.to = tokens[index++];
  if (states.find(transition.to) == states.end()) {
    throw InvalidDefinitionException(
        line.number, "state '" + transition.to + "' is not in Q");
  }

  for (int i = 0; i < tape_count; ++i) {
    const std::string& token = tokens[index++];
    if (token.size() != 1 || !tape_alphabet.Contains(token[0])) {
      throw InvalidDefinitionException(
          line.number, "'" + token + "' is not in Gamma");
    }
    transition.write.push_back(token[0]);
  }

  for (int i = 0; i < tape_count; ++i) {
    transition.move.push_back(ParseMovement(line, tokens[index++]));
  }

  return transition;
}

/**
 * @brief Whether a and b agree on destination, writes and moves, i.e.
 * one is a harmless exact duplicate of the other.
 */
bool SameOutcome(const TMTransition& a, const TMTransition& b) {
  return a.to == b.to && a.write == b.write && a.move == b.move;
}

/**
 * @brief Adds transition to table, after checking it against whatever
 * transition (if any) already applies to the same (state, read): an
 * exact duplicate is skipped with a warning, a genuine conflict (same
 * state and read, different outcome) is a malformed, non-deterministic
 * definition and fails immediately with the offending line number.
 */
void AddTransition(TMTransitionTable* table, const RawLine& line,
                    const TMTransition& transition) {
  std::optional<TMTransition> existing =
      table->Find(transition.from, transition.read);
  if (existing.has_value()) {
    if (SameOutcome(*existing, transition)) {
      PrintWarning("line " + std::to_string(line.number) +
                   ": duplicate transition ignored");
      return;
    }
    throw InvalidDefinitionException(
        line.number, "state '" + transition.from + "' already has a "
                      "different transition for the same symbols read, "
                      "which is not allowed in a deterministic machine");
  }
  table->Add(transition);
}

}  // namespace

TuringMachine ParseTuringMachineFile(const std::string& filename) {
  std::vector<RawLine> lines = ReadMeaningfulLines(filename);
  if (lines.size() < 7) {
    throw InvalidDefinitionException(
        lines.back().number,
        "the definition must include Q, Sigma, Gamma, the initial state, "
        "the blank symbol, F and the number of tapes");
  }

  size_t index = 0;
  std::set<std::string> states = ParseStateSet(lines[index]);
  if (states.empty()) {
    throw InvalidDefinitionException(lines[index].number, "Q must not be empty");
  }
  ++index;

  Alphabet input_alphabet = ParseAlphabet(lines[index++], "Sigma");
  Alphabet tape_alphabet = ParseAlphabet(lines[index++], "Gamma");

  std::vector<std::string> initial_state_tokens = Tokenize(lines[index].content);
  if (initial_state_tokens.size() != 1) {
    throw InvalidDefinitionException(
        lines[index].number, "the initial state must be a single token");
  }
  std::string initial_state = initial_state_tokens[0];
  if (states.find(initial_state) == states.end()) {
    throw InvalidDefinitionException(
        lines[index].number,
        "initial state '" + initial_state + "' is not in Q");
  }
  ++index;

  char blank_symbol = ParseSingleSymbol(lines[index], "blank symbol");
  if (!tape_alphabet.Contains(blank_symbol)) {
    throw InvalidDefinitionException(
        lines[index].number, "blank symbol '" + std::string(1, blank_symbol) +
                                  "' is not in Gamma");
  }
  ++index;

  std::set<std::string> final_states = ParseFinalStates(lines[index], states);
  ++index;

  int tape_count = ParseTapeCount(lines[index]);
  ++index;

  if (index >= lines.size()) {
    throw InvalidDefinitionException(lines.back().number,
                                      "the definition has no transitions");
  }

  TMTransitionTable transitions;
  for (; index < lines.size(); ++index) {
    TMTransition transition =
        ParseTransition(lines[index], states, tape_alphabet, tape_count);
    AddTransition(&transitions, lines[index], transition);
  }

  return TuringMachine(states, input_alphabet, tape_alphabet, initial_state,
                        blank_symbol, final_states, tape_count, transitions);
}
