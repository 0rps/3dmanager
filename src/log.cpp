#include "log.h"

#include <QDebug>

static Log staticErrorLog("ERROR", Qt::red);
static Log staticWarningLog("INFO", Qt::yellow);
static Log staticDebugLog("DEBUG", Qt::black);

Log& logError()
{
   if (false ==  staticErrorLog.isNewLine())
       staticErrorLog.addLine();
   return staticErrorLog;
}

Log& logInfo()
{
   if (false ==  staticWarningLog.isNewLine())
       staticWarningLog.addLine();
   return staticDebugLog;
}

Log& logDebug()
{
   if (false ==  staticDebugLog.isNewLine())
       staticDebugLog.addLine();
   return staticDebugLog;
}


Log &Log::operator <<(const QString &_str)
{
    writeToLog(_str);
    return *this;
}

Log &Log::operator <<(double _value)
{
    writeToLog(QString::number(_value));
    return *this;
}

Log &Log::operator <<(int _value)
{
    writeToLog(QString::number(_value));
    return *this;
}

Log &Log::operator <<(long _value)
{
    writeToLog(QString::number(_value));
    return *this;
}

void Log::addLine()
{
    m_newLine = true;
}

bool Log::isNewLine()
{
    return m_newLine;
}

Log::Log(const QString &_prefix, const QColor &_color):
    m_newLine(true)
{
}

void Log::writeToLog(const QString &_str)
{
    if (m_log.isNull())
    {
        qDebug() << "OLOLO: No logger edit is setted";
        qDebug() << _str;
        return;
    }
    m_mutex.lock();
    m_log->setTextColor(m_color);
    if (m_newLine) {
        writeString("\n" + m_prefix + ": ");
        m_newLine = false;
    }
    writeString(_str);
    m_mutex.unlock();
}

void Log::writeString(const QString &_text)
{
    m_log->moveCursor (QTextCursor::End);
    m_log->insertPlainText (_text);
    m_log->moveCursor (QTextCursor::End);
}


void setLogTextEdit(QTextEdit *_edit)
{
    Log::m_log = _edit;
}
