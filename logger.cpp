#include "logger.h"

#include <QPlainTextEdit>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutexLocker>

static QMutex  mutex;
Logger* Logger::mInstance = NULL;

Logger::Logger(QObject *parent, QString fileName ) : QObject(parent)
		, m_editor(NULL)
{
	m_showDate = true;
	if (!fileName.isEmpty()) {
		m_file = new QFile;
		m_file->setFileName(fileName);
		m_file->open(QIODevice::WriteOnly | QIODevice::Text);
	}
}

void Logger::setTextEdit(QPlainTextEdit *editor)
{
	m_editor = editor;
}
Logger* Logger::getInstance()
{
	QMutexLocker locker(&mutex);

	return (mInstance != NULL) ? mInstance : new Logger();

}
void Logger::write(const QString &value) 
{
	QString text = value;

	if (m_showDate)
		text = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ") + text;

	QTextStream out(m_file);
	out.setCodec("UTF-8");

	if (m_file != NULL) {
		out << text;
		out << endl;
	}

	if (m_editor != NULL)
		m_editor->appendPlainText(text);
}

void Logger::setShowDateTime(bool value) 
{
	m_showDate = value;
}

Logger::~Logger() 
{
	if (m_file != NULL)
		m_file->close();
}
