#ifndef PATTERNEMITTER_H
#define PATTERNEMITTER_H
#define PATTERN_TIMER why_are_you_trying

#include <QObject>
#include <QTimer>
#include "mizushirushihandora.h"

class PatternEmitter : public QObject
{
    Q_OBJECT
public:
    //PatternEmitter(QObject *parent = nullptr);
    PatternEmitter(const std::string &phoneNumber,int duration ,QObject *parent = nullptr);
    void start(int duration);
private slots:
    void emitNextSignal();

private:
    QTimer *PATTERN_TIMER;
    std::map<char, std::string> morseCodeMapping;
    std::string morseCodeString;
    std::vector<std::string> morseSequence;
    int current_index;
    DFlashMizu* config = nullptr;


    std::vector<std::string> convertToSequence(const std::string &morseCode);
    std::vector<std::string> convertToSequence(const std::string &morseCode, int duration);
    QString &getSpacedOutString(QString &stringToSpaceOut);
signals:
    void emitDot();
    void emitDash();
    void emitSpace();
    void emitDollar();

};

#endif // PATTERNEMITTER_H
