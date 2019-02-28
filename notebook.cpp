#include <QApplication>
#include <QWidget>
#include <QDebug>
#include <QProcess>
#include <QTextStream>
#include "notebook_app.hpp"


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QWidget widget;
  NotebookApp nApp;

  nApp.show();
  
  return app.exec();
}

