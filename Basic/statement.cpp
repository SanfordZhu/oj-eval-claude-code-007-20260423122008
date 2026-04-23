/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"
#include <iostream>
#include <cstdlib>

/* Implementation of the Statement class */

int stringToInt(std::string str);

Statement::Statement() = default;

Statement::~Statement() = default;

/* Implementation of RemStatement */

RemStatement::RemStatement(const std::string& comment) : comment(comment) {}

void RemStatement::execute(EvalState &state, Program &program) {
    // REM statements do nothing
}

/* Implementation of LetStatement */

LetStatement::LetStatement(const std::string& var, Expression* exp) : var(var), exp(exp) {}

LetStatement::~LetStatement() {
    delete exp;
}

void LetStatement::execute(EvalState &state, Program &program) {
    int value = exp->eval(state);
    state.setValue(var, value);
}

/* Implementation of PrintStatement */

PrintStatement::PrintStatement(Expression* exp) : exp(exp) {}

PrintStatement::~PrintStatement() {
    delete exp;
}

void PrintStatement::execute(EvalState &state, Program &program) {
    int value = exp->eval(state);
    std::cout << value << std::endl;
}

/* Implementation of InputStatement */

InputStatement::InputStatement(const std::string& var) : var(var) {}

void InputStatement::execute(EvalState &state, Program &program) {
    int value;
    std::cin >> value;
    state.setValue(var, value);
}

/* Implementation of EndStatement */

EndStatement::EndStatement() {}

void EndStatement::execute(EvalState &state, Program &program) {
    std::exit(0);
}

/* Implementation of GotoStatement */

GotoStatement::GotoStatement(int lineNumber) : lineNumber(lineNumber) {}

void GotoStatement::execute(EvalState &state, Program &program) {
    // This will be handled by the program execution loop
    throw ErrorException("GOTO " + std::to_string(lineNumber));
}

/* Implementation of IfStatement */

IfStatement::IfStatement(Expression* exp1, const std::string& op, Expression* exp2, int lineNumber)
    : exp1(exp1), op(op), exp2(exp2), lineNumber(lineNumber) {}

IfStatement::~IfStatement() {
    delete exp1;
    delete exp2;
}

void IfStatement::execute(EvalState &state, Program &program) {
    int val1 = exp1->eval(state);
    int val2 = exp2->eval(state);
    bool condition = false;

    if (op == "=") {
        condition = (val1 == val2);
    } else if (op == "<") {
        condition = (val1 < val2);
    } else if (op == ">") {
        condition = (val1 > val2);
    } else if (op == "<=") {
        condition = (val1 <= val2);
    } else if (op == ">=") {
        condition = (val1 >= val2);
    } else if (op == "<>") {
        condition = (val1 != val2);
    }

    if (condition) {
        throw ErrorException("GOTO " + std::to_string(lineNumber));
    }
}

/* Implementation of RunStatement */

RunStatement::RunStatement() {}

void RunStatement::execute(EvalState &state, Program &program) {
    int currentLine = program.getFirstLineNumber();

    while (currentLine != -1) {
        Statement* stmt = program.getParsedStatement(currentLine);
        if (stmt == nullptr) {
            // Parse the statement if not already parsed
            std::string line = program.getSourceLine(currentLine);
            TokenScanner scanner;
            scanner.ignoreWhitespace();
            scanner.scanNumbers();
            scanner.setInput(line);

            // Skip line number
            scanner.nextToken();

            std::string command = scanner.nextToken();
            // Parse and create statement based on command
            // This is a simplified version - full parsing would go here
        }

        try {
            stmt->execute(state, program);
            currentLine = program.getNextLineNumber(currentLine);
        } catch (ErrorException& ex) {
            std::string msg = ex.getMessage();
            if (msg.substr(0, 5) == "GOTO ") {
                int targetLine = stringToInteger(msg.substr(5));
                if (program.getSourceLine(targetLine) == "") {
                    error("LINE NUMBER ERROR");
                }
                currentLine = targetLine;
            } else {
                throw;
            }
        }
    }
}

/* Implementation of ListStatement */

ListStatement::ListStatement() {}

void ListStatement::execute(EvalState &state, Program &program) {
    std::vector<int> lineNumbers = program.getAllLineNumbers();
    for (int lineNum : lineNumbers) {
        std::cout << program.getSourceLine(lineNum) << std::endl;
    }
}

/* Implementation of ClearStatement */

ClearStatement::ClearStatement() {}

void ClearStatement::execute(EvalState &state, Program &program) {
    program.clear();
    state.Clear();
}

/* Implementation of QuitStatement */

QuitStatement::QuitStatement() {}

void QuitStatement::execute(EvalState &state, Program &program) {
    std::exit(0);
}
