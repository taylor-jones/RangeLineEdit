#ifndef SRC_COORDINATE_EDIT
#define SRC_COORDINATE_EDIT

#include <QLineEdit>

class QKeyEvent;
class QWheelEvent;
class QPaintEvent;
class QMouseEvent;

class CoordinateEdit : public QLineEdit {
 public:
  CoordinateEdit(QWidget* parent = nullptr);
  ~CoordinateEdit();

  double value();

 public slots:
  void clearText();
  void setValue(double value);
  void setDecimals(int decimals);

 private slots:
  void valueChangedPrivate();
  void createCustomContextMenu();

 signals:
  void valueChanged(double value);

 private:
  void keyPressEvent(QKeyEvent* event) override;
  void paintEvent(QPaintEvent* paintEvent) override;
  void wheelEvent(QWheelEvent* wheelEvent) override;
  void cursorPositionChangedEvent(int, int cur);
  void selectionChangedEvent();

  void copyTextToClipboard();
  void copyValueToClipboard();
  void pasteValueFromClipboard();
  void setText(const QString& text);

};

#endif // SRC_COORDINATE_EDIT
