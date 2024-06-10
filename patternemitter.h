#ifndef PATTERNEMITTER_H
#define PATTERNEMITTER_H
#define PATTERN_TIMER why_are_you_trying

#include <QObject>
#include <QTimer>

class PatternEmitter : public QObject
{
    Q_OBJECT
public:
    //PatternEmitter(QObject *parent = nullptr);
    PatternEmitter(const std::string &phoneNumber, QObject *parent = nullptr);
    void start();

private slots:
    void emitNextSignal();

private:
    QTimer *PATTERN_TIMER;
    std::map<char, std::string> morseCodeMapping;
    std::string morseCodeString;
    std::vector<std::string> morseSequence;
    int current_index;

    std::vector<std::string> convertToSequence(const std::string &morseCode);
signals:
    void emitDot();
    void emitDash();
    void emitSpace();
};

#endif // PATTERNEMITTER_H
