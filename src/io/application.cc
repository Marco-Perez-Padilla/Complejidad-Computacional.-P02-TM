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

** File application.cc: implementation of Application.
**/

#include "io/application.h"

#include <fstream>
#include <utility>

#include "exceptions/exceptions.h"
#include "io/menu.h"
#include "io/turing_machine_parser.h"
#include "io/word_checker.h"

Application::Application(Options options, std::istream& in, std::ostream& out)
    : options_(std::move(options)), in_(in), out_(out) {}

TuringMachine Application::LoadMachine() const {
  TuringMachine machine = ParseTuringMachineFile(options_.config_file);
  out_ << "Machine loaded: " << machine.States().size() << " states, "
       << machine.TapeCount() << " tape(s), " << machine.FinalStates().size()
       << " accepting state(s)\n";
  return machine;
}

std::ostream& Application::SelectTraceStream(std::ofstream& trace_file) const {
  if (options_.trace && options_.output_file.has_value()) {
    trace_file.open(*options_.output_file);
    if (!trace_file.is_open()) {
      throw FileNotFoundException(*options_.output_file);
    }
    return trace_file;
  }
  return out_;
}

void Application::Run() {
  TuringMachine machine = LoadMachine();

  // trace_file lives here, in Run's scope, so it stays valid for as long
  // as the WordChecker/Menu built below might write a trace through it.
  std::ofstream trace_file;
  std::ostream& trace_out = SelectTraceStream(trace_file);

  WordChecker checker(machine, options_.trace, out_, trace_out);

  if (options_.input_file.has_value()) {
    checker.CheckAllFromFile(*options_.input_file);
  } else {
    Menu menu(checker, in_, out_);
    menu.Run();
  }
}
