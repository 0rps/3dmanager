#ifndef LOG_H
#define LOG_H

#include <QTextEdit>
#include <QPointer>
#include <QMutex>

class Log
{
    friend void setLogTextEdit(QTextEdit *_edit);
public:
    Log& operator <<(const QString &_str);
    Log& operator <<(double _value);
    Log& operator <<(int _value);
    Log& operator <<(long _value);
    void addLine();
    bool isNewLine();
protected:
    Log(const QString &_prefix, const QColor &_color);
    void writeToLog(const QString &_str);
    void writeString(const QString &_text);
private:
    static QPointer<QTextEdit> m_log;
    QString m_prefix;
    QColor m_color;
    bool m_newLine;
    static QMutex m_mutex;

};

Log& logError();
Log& logInfo();
Log& logDebug();

void setLogTextEdit(QTextEdit *_edit);

#endif // LOG_H
