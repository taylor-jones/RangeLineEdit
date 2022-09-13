#include <QApplication>
#include <QDebug>
#include <QList>
#include <functional>

#include "MainWindow.h"
#include "line_part.h"

int main(int argc, char* argv[]){
  QApplication app(argc, argv);

  QList<LinePart*> parts;

  auto* sign_part = new CharLinePart(QChar{}, QChar{'-'});
  auto* dot_part = new ConstStringLinePart(".");
  auto* degree_symbol_part = new ConstStringLinePart("°");
  auto* space_symbol_part = new ConstStringLinePart(" ");
  auto* minutes_symbol_part = new ConstStringLinePart("'");
  auto* seconds_symbol_part = new ConstStringLinePart("\"");

  auto* int_part = new Int64LinePart(90.0, 1.0);
  auto* fract_part = new Int64LinePart(std::pow(10LL, 4) - 1LL, std::pow(10LL, 4));


  parts << sign_part << int_part << dot_part << fract_part;

  qDebug() << parts;

  delete sign_part;
  delete dot_part;
  delete degree_symbol_part;
  delete space_symbol_part;
  delete minutes_symbol_part;
  delete seconds_symbol_part;
  delete int_part;
  delete fract_part;

  MainWindow w;
  w.show();
  return app.exec();
}
