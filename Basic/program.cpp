/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"
#include <map>



Program::Program() = default;

Program::~Program() {
    clear();
}

void Program::clear() {
    for (auto& pair : lines) {
        if (pair.second.statement != nullptr) {
            delete pair.second.statement;
        }
    }
    lines.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    auto it = lines.find(lineNumber);
    if (it != lines.end()) {
        // Line exists, replace it
        if (it->second.statement != nullptr) {
            delete it->second.statement;
            it->second.statement = nullptr;
        }
        it->second.sourceLine = line;
    } else {
        // New line
        Line newLine;
        newLine.sourceLine = line;
        newLine.statement = nullptr;
        lines[lineNumber] = newLine;
    }
}

void Program::removeSourceLine(int lineNumber) {
    auto it = lines.find(lineNumber);
    if (it != lines.end()) {
        if (it->second.statement != nullptr) {
            delete it->second.statement;
        }
        lines.erase(it);
    }
}

std::string Program::getSourceLine(int lineNumber) {
    auto it = lines.find(lineNumber);
    if (it != lines.end()) {
        return it->second.sourceLine;
    }
    return "";
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    auto it = lines.find(lineNumber);
    if (it == lines.end()) {
        error("Line number does not exist");
    }
    if (it->second.statement != nullptr) {
        delete it->second.statement;
    }
    it->second.statement = stmt;
}

//void Program::removeSourceLine(int lineNumber) {

Statement *Program::getParsedStatement(int lineNumber) {
    auto it = lines.find(lineNumber);
    if (it != lines.end()) {
        return it->second.statement;
    }
    return nullptr;
}

int Program::getFirstLineNumber() {
    if (lines.empty()) {
        return -1;
    }
    return lines.begin()->first;
}

int Program::getNextLineNumber(int lineNumber) {
    auto it = lines.find(lineNumber);
    if (it == lines.end()) {
        return -1;
    }
    ++it;
    if (it == lines.end()) {
        return -1;
    }
    return it->first;
}

std::vector<int> Program::getAllLineNumbers() {
    std::vector<int> result;
    for (const auto& pair : lines) {
        result.push_back(pair.first);
    }
    return result;
}

//more func to add
//todo


