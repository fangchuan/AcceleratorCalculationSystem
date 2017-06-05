#pragma once

#include <QObject>
#include <QPlainTextEdit>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class Logger : public QObject
{
	Q_OBJECT
public:

	~Logger();
	void setShowDateTime(bool value);
	void setTextEdit(QPlainTextEdit *editor);
	static Logger* getInstance();

private:
	explicit Logger(QObject *parent = NULL,  QString fileName = "LNAC.log");
	
	static Logger*  mInstance;
	QFile *m_file;
	QPlainTextEdit *m_editor;
	bool m_showDate;

signals:

public slots :
		void write(const QString &value);

};
