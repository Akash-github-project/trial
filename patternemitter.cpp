#include "patternemitter.h"

PatternEmitter::PatternEmitter(const std::string &phoneNumber,int duration ,QObject *parent)
    : QObject(parent), current_index(0) {
    qDebug()<<phoneNumber << "hello 123";
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
        morseCodeString += morseCodeMapping[digit] + "$";
    }
    morseCodeString += QString("$").toStdString();
    QString morseCodeQs = QString::fromStdString(morseCodeString);
    morseSequence = convertToSequence(getSpacedOutString(morseCodeQs).toStdString(),duration);
    PATTERN_TIMER = new QTimer(this);
    PATTERN_TIMER->setTimerType(Qt::PreciseTimer);
    connect(PATTERN_TIMER, &QTimer::timeout, this, &PatternEmitter::emitNextSignal);
}


QString& PatternEmitter::getSpacedOutString(QString& stringToSpaceOut){

    int len = stringToSpaceOut.length();
    int i = 0;

    // Iterate through the string and insert spaces when needed
    while (i < len) {
        if (stringToSpaceOut[i] != '$') {
            // Insert a space after the current character if it's not '$'
            stringToSpaceOut.insert(i + 1, ' ');
            len++; // Since we've inserted a character, increase the length
            i += 2; // Move to the next character after the inserted space
        } else {
            i++; // Skip the '$' without inserting a space
        }
    }
    return stringToSpaceOut;  // Return the modified string reference
}

void PatternEmitter::start(int duration) {
    PATTERN_TIMER->start(1000); // Adjust timing for dot length
}

// void PatternEmitter::emitNextSignal() {
//     if (current_index < morseSequence.size()) {
//         std::string signal = morseSequence[current_index];
//         if (signal == "dot") {
//             emit emitDot();
//         } else if (signal == "dash") {
//             emit emitDash();
//         } else  {
//             emit emitSpace();
//         }
//         current_index++;
//     } else {
//         current_index = 0;
//     }
// }

void PatternEmitter::emitNextSignal() {
    if (current_index < morseSequence.size()) {
        std::string signal = morseSequence[current_index];
        if (signal == "dot") {
            emit emitDot();
        } else if (signal == "dash") {
            emit emitDash();
        } else if(signal == "space"){
            emit emitSpace();
        } else if(signal == "dollar"){
            emit emitDollar();
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


std::vector<std::string> PatternEmitter::convertToSequence(const std::string &morseCode,int duration) {
    std::vector<std::string> sequence;
    qDebug()<<QString::fromStdString(morseCode);
    for (char c : morseCode) {
        if (c == '.') {
            for(int i = 0;i<duration; i++ ){
                sequence.push_back("dot");
                //qDebug().nospace()<<".";
            }
        } else if (c == '-') {
            for(int i = 0;i<duration; i++ ){
                sequence.push_back("dash");
                //qDebug().nospace()<<"_";
            }
        } else if(c == ' '){
            for(int i = 0;i<duration; i++ ){
                sequence.push_back("space");
                //qDebug().nospace()<<"sp";
            }
        }else if(c == '$') {
            for(int i = 0;i<duration * 2; i++ ){
                sequence.push_back("dollar");
                //qDebug().nospace()<<"sp";
            }
        }
    }

    return sequence;
}
