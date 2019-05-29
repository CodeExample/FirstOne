#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
#ifdef HAVE_QT5
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
  QApplication a(argc, argv);

  QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));

  QApplication::setStyle(QStyleFactory::create("Fusion"));

/*  QPalette darkPalette;

  darkPalette.setColor(QPalette::Window, QColor(67,70,80));
  darkPalette.setColor(QPalette::WindowText, Qt::white);
  darkPalette.setColor(QPalette::Base, QColor(25,25,25));
  darkPalette.setColor(QPalette::AlternateBase, QColor(67,70,80));
  darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
  darkPalette.setColor(QPalette::ToolTipText, Qt::white);
  darkPalette.setColor(QPalette::Text, Qt::white);
  darkPalette.setColor(QPalette::Button, QColor(67,70,80));
  darkPalette.setColor(QPalette::ButtonText, Qt::white);
  darkPalette.setColor(QPalette::BrightText, Qt::red);
  darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

  darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
  darkPalette.setColor(QPalette::HighlightedText, Qt::black);

  qApp->setPalette(darkPalette);

  //qApp->setStyleSheet("QUndoView {background-color: #424851;} QTextEdit {background-color: #424851;} QTreeView {background-color: #424851;} QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; } QPushButton:disabled{background-color: '#434650'; color: 'gray'}");
*/

  MainWindow w;
  w.show();

  return a.exec();
}
