#ifndef SRC_COORDINATE_EDIT_P
#define SRC_COORDINATE_EDIT_P

#include <QList>
#include <QObject>

class CoordinateEdit;
class LinePart;

class QChar;
class QColor;
class QWidget;


class CoordinateEditPrivate {
 public:
  CoordinateEditPrivate(CoordinateEdit* parent = nullptr);
  ~CoordinateEditPrivate();

  void setDecimals(int decimals);
  bool setValueAtIndex(const QChar& value, int index);
  void setActiveIndexHighlightColor(const QColor& color, bool translucent = true);

  LinePart* nearestMutablePartBefore();
  LinePart* nearestMutablePartAfter();
  LinePart* partAtIndex(int index);

  void scrapeTextFromLinePartValue(LinePart* part, bool override_being_dirty = false);
  void scrapeDirtyLineParts(bool override_being_dirty = false);
  void syncLinePartEdges();
  void syncLinePartSigns();
  long double sumLineParts();

  void incrementPart(int index);
  void decrementPart(int index);

  // wrappers around increment/decrement part functions that use the current cursor index.
  void incrementCursorPart();
  void decrementCursorPart();

  void moveToPrevMutablePart();
  void moveToNextMutablePart();

 public:
  QList<LinePart*> parts_;

 private:
  CoordinateEdit* p = nullptr;
};

#endif // SRC_COORDINATE_EDIT_P
