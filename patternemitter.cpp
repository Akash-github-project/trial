#include "patternemitter.h"

PatternEmitter::PatternEmitter(const std::string &phoneNumber, QObject *parent)
    : QObject(parent), current_index(0) {
    morseCodeMapping = {
        {'0', "-----"},
        {'1', ".----"},
        {'2', "..---"},
        {'3', "...--"},
        {'4', "....-"},
        {'5', "....."},
        {'6', "-...."},
        {'7', "--..."},
        {'8', "---.."},
        {'9', "----."}
    };
    for (char digit : phoneNumber) {
        morseCodeString += morseCodeMapping[digit] + " ";
    }
    morseSequence = convertToSequence(morseCodeString);
    PATTERN_TIMER = new QTimer(this);
    connect(PATTERN_TIMER, &QTimer::timeout, this, &PatternEmitter::emitNextSignal);
}

void PatternEmitter::start() {
    PATTERN_TIMER->start(1000); // Adjust timing for dot length
}

void PatternEmitter::emitNextSignal() {
    if (current_index < morseSequence.size()) {
        std::string signal = morseSequence[current_index];
        if (signal == "dot") {
            emit emitDot();
        } else if (signal == "dash") {
            emit emitDash();
        } else  {
            emit emitSpace();
        }
        current_index++;
    } else {
        current_index = 0;
    }
}

std::vector<std::string> PatternEmitter::convertToSequence(const std::string &morseCode) {
    std::vector<std::string> sequence;
    for (char c : morseCode) {
        if (c == '.') {
            sequence.push_back("dot");
        } else if (c == '-') {
            sequence.push_back("dash");
        } else {
            sequence.push_back("space");
        }
    }
    return sequence;
}
