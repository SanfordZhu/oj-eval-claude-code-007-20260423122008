/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include <cstdlib>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "statement.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            if (!getline(std::cin, input)) {
                // EOF reached - exit gracefully
                break;
            }
            if (input.empty())
                continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

// Forward declarations
Expression *readE(TokenScanner &scanner, int prec);
int precedence(std::string token);

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    // Check if this is a line number (program line)
    std::string firstToken = scanner.nextToken();
    TokenType firstType = scanner.getTokenType(firstToken);

    if (firstType == NUMBER && firstToken.find_first_not_of("0123456789") == std::string::npos) {
        int lineNumber = stringToInteger(firstToken);

        if (scanner.hasMoreTokens()) {
            // This is a program line with content
            program.addSourceLine(lineNumber, line);

            // Get the command
            std::string command = scanner.nextToken();
            Statement* stmt = nullptr;

            if (command == "REM") {
                // Get the rest of the line as comment
                std::string comment;
                while (scanner.hasMoreTokens()) {
                    if (!comment.empty()) comment += " ";
                    comment += scanner.nextToken();
                }
                stmt = new RemStatement(comment);
            } else if (command == "LET") {
                std::string var = scanner.nextToken();
                if (scanner.nextToken() != "=") {
                    error("INVALID STATEMENT");
                }
                Expression* exp = parseExp(scanner);
                stmt = new LetStatement(var, exp);
            } else if (command == "PRINT") {
                if (!scanner.hasMoreTokens()) {
                    error("INVALID STATEMENT");
                }
                Expression* exp = parseExp(scanner);
                stmt = new PrintStatement(exp);
            } else if (command == "INPUT") {
                std::string var = scanner.nextToken();
                stmt = new InputStatement(var);
            } else if (command == "END") {
                stmt = new EndStatement();
            } else if (command == "GOTO") {
                int targetLine = stringToInteger(scanner.nextToken());
                stmt = new GotoStatement(targetLine);
            } else if (command == "IF") {
                // Parse IF statement manually
                std::vector<std::string> tokens;
                while (scanner.hasMoreTokens()) {
                    tokens.push_back(scanner.nextToken());
                }

                // Find THEN
                size_t thenPos = 0;
                for (size_t i = 0; i < tokens.size(); i++) {
                    if (tokens[i] == "THEN") {
                        thenPos = i;
                        break;
                    }
                }
                if (thenPos == 0 || thenPos == tokens.size() - 1) {
                    error("INVALID STATEMENT");
                }

                // Parse expressions
                TokenScanner exp1Scanner;
                exp1Scanner.ignoreWhitespace();
                exp1Scanner.scanNumbers();
                for (size_t i = 0; i < thenPos - 2; i++) {
                    exp1Scanner.saveToken(tokens[i]);
                }
                Expression* exp1 = parseExp(exp1Scanner);

                std::string op = tokens[thenPos - 2];

                TokenScanner exp2Scanner;
                exp2Scanner.ignoreWhitespace();
                exp2Scanner.scanNumbers();
                exp2Scanner.saveToken(tokens[thenPos - 1]);
                Expression* exp2 = parseExp(exp2Scanner);

                int targetLine = stringToInteger(tokens[thenPos + 1]);
                stmt = new IfStatement(exp1, op, exp2, targetLine);
            } else {
                error("INVALID STATEMENT");
            }

            program.setParsedStatement(lineNumber, stmt);
        } else {
            // Empty line number - remove the line
            program.removeSourceLine(lineNumber);
        }
    } else {
        // This is a direct command
        scanner.saveToken(firstToken);
        std::string command = scanner.nextToken();

        if (command == "RUN") {
            RunStatement stmt;
            stmt.execute(state, program);
        } else if (command == "LIST") {
            ListStatement stmt;
            stmt.execute(state, program);
        } else if (command == "CLEAR") {
            ClearStatement stmt;
            stmt.execute(state, program);
        } else if (command == "QUIT") {
            QuitStatement stmt;
            stmt.execute(state, program);
        } else if (command == "HELP") {
            // Optional - not tested
            std::cout << "BASIC Interpreter Commands:" << std::endl;
            std::cout << "RUN, LIST, CLEAR, QUIT, HELP" << std::endl;
        } else {
            // Try to parse as immediate statement
            if (command == "REM") {
                // Immediate REM - just ignore
            } else if (command == "LET") {
                std::string var = scanner.nextToken();
                if (scanner.nextToken() != "=") {
                    error("INVALID STATEMENT");
                }
                Expression* exp = parseExp(scanner);
                if (exp == nullptr) {
                    error("INVALID EXPRESSION");
                }
                LetStatement stmt(var, exp);
                stmt.execute(state, program);
                // Don't delete exp - LetStatement owns it
            } else if (command == "PRINT") {
                Expression* exp = parseExp(scanner);
                PrintStatement stmt(exp);
                stmt.execute(state, program);
                // Don't delete exp - PrintStatement will delete it in its destructor
            } else if (command == "INPUT") {
                std::string var = scanner.nextToken();
                InputStatement stmt(var);
                stmt.execute(state, program);
            } else if (command == "END") {
                EndStatement stmt;
                stmt.execute(state, program);
            } else {
                error("INVALID STATEMENT");
            }
        }
    }
}

