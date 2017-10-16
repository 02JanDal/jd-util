#pragma once

#include <QMainWindow>

namespace JD {
namespace Util {

class MainWindow;

class AboutBuilder
{
public:
	explicit AboutBuilder(MainWindow *win);

	AboutBuilder setContact(const QString &contact);
	AboutBuilder setCopyrightDate(const QString &date);
	AboutBuilder addLicense(const QString &name, const QString &text);

	void build();

private:
	MainWindow *m_window;
	QString m_contact;
	QString m_copyrightDate;
	QVector<QPair<QString ,QString>> m_licenses;
};

class MainWindow : public QMainWindow
{
public:
	explicit MainWindow(QWidget *parent);
	virtual ~MainWindow();

	void setAboutText(const QString &text);
	AboutBuilder buildAbout();

public slots:
	void showAbout();
	void showAboutQt();
	void quit();

private:
	QString m_aboutText;
};

}
}
