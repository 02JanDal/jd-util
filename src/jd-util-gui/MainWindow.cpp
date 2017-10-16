#include "MainWindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QDate>
#include <QSettings>

namespace JD {
namespace Util {

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("MainWindow"));
	restoreGeometry(settings.value(QStringLiteral("Geometry")).toByteArray());
	restoreState(settings.value(QStringLiteral("State")).toByteArray());
}
MainWindow::~MainWindow()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("MainWindow"));
	settings.setValue(QStringLiteral("Geometry"), saveGeometry());
	settings.setValue(QStringLiteral("State"), saveState());
}

void MainWindow::setAboutText(const QString &text)
{
	m_aboutText = text;
}
AboutBuilder MainWindow::buildAbout()
{
	return AboutBuilder(this);
}

void MainWindow::showAbout()
{
	QMessageBox::about(this, tr("About %1").arg(qApp->applicationDisplayName()), m_aboutText);
}
void MainWindow::showAboutQt()
{
	QMessageBox::aboutQt(this);
}
void MainWindow::quit()
{
	qApp->quit();
}

AboutBuilder::AboutBuilder(MainWindow *win)
	: m_window(win), m_copyrightDate(QString::number(QDate::currentDate().year())) {}

AboutBuilder AboutBuilder::setContact(const QString &contact)
{
	m_contact = contact;
	return *this;
}
AboutBuilder AboutBuilder::setCopyrightDate(const QString &date)
{
	m_copyrightDate = date;
	return *this;
}
AboutBuilder AboutBuilder::addLicense(const QString &name, const QString &text)
{
	m_licenses.append(qMakePair(name, text));
	return *this;
}

void AboutBuilder::build()
{
	QString text = MainWindow::tr("<h1>%1 <small>%2</small></h1>").arg(qApp->applicationDisplayName(), qApp->applicationVersion());
	text += MainWindow::tr("<p>Copyright %copy; %1 %2</p>").arg(m_copyrightDate, qApp->organizationName());
	if (!m_contact.isEmpty()) {
		text += MainWindow::tr("<h5>Contact</h5><p>%1</p>").arg(m_contact);
	}
	if (!m_licenses.isEmpty()) {
		text += MainWindow::tr("<h5>Licenses</h5>");
		for (const auto &pair : m_licenses) {
			text += QStringLiteral("<h6>%1</h6><code>%2</code>").arg(pair.first, pair.second);
		}
	}
	m_window->setAboutText(text);
}

}
}
