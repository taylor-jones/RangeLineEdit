#ifndef SRC_LINE_PART
#define SRC_LINE_PART

// #include <QObject>
#include <QChar>
#include <QDebug>
#include <QRegExp>
#include <QString>
#include <functional>

enum class LinePartType : int {
  CharPart,
  ConstStringPart,
  Int64Part,
};


class LinePart {
 public:
  inline virtual ~LinePart() = default;

  virtual int valueLength() const = 0;
  virtual int maxValueLength() const = 0;
  virtual LinePartType partType() const = 0;
  virtual QString valueString() const = 0;

  inline LinePart* prevPart() { return prev_; }
  inline LinePart* nextPart() { return next_; }

  inline void setPrevPart(LinePart* prev) { prev_ = prev; }
  inline void setNextPart(LinePart* next) { next_ = next; }

 private:
  LinePart* prev_ = nullptr;
  LinePart* next_ = nullptr;
};


class CharLinePart : public LinePart {
 public:
  inline CharLinePart(const QChar& positive_char, const QChar& negative_char)
    : positive_char_{positive_char},
      negative_char_{negative_char},
      value_{positive_char} {}
  inline ~CharLinePart() override = default;

  inline int valueLength() const override { return 1; }
  inline int maxValueLength() const override { return 1; }
  inline LinePartType partType() const override { return LinePartType::CharPart; }
  inline QString valueString() const override { return QString{value_}; }

  inline QChar value() const { return value_; }
  inline QChar positiveChar() const { return positive_char_; }
  inline QChar negativeChar() const { return negative_char_; }

  inline void setValue(bool positive) { value_ = positive ? positive_char_ : negative_char_; }

 private:
  QChar value_;
  const QChar positive_char_;
  const QChar negative_char_;
};



class ConstStringLinePart : public LinePart {
 public:
  inline ConstStringLinePart(const QString& str) {}
  inline ~ConstStringLinePart() override = default;

  inline int valueLength() const override { return value_.length(); }
  inline int maxValueLength() const override { return value_.length(); }
  inline LinePartType partType() const override { return LinePartType::ConstStringPart; }
  inline QString valueString() const override { return value_; }
  inline QString value() { return value_; }

 private:
  const QString value_;
};



class Int64LinePart : public LinePart {
 public:
  Int64LinePart(uint64_t range, int64_t divisor);
  inline ~Int64LinePart() override = default;

  inline int valueLength() const override { return QString::number(llabs(value_)).length(); }
  inline int maxValueLength() const override { return QString::number(range_).length(); }
  inline LinePartType partType() const override { return LinePartType::Int64Part; }
  inline QString valueString() const override { return QString("0").repeated(maxValueLength() - valueLength()) + QString::number(llabs(value_)); }
  inline int64_t value() { return value_; }

  void setRange(uint64_t range);
  void setDivisor(uint64_t divisor);
  void setValue(int64_t value);
  bool setValueAtIndex(const QChar& value, int index);

  bool increment(int index);
  bool decrement(int index);

 private:
  int64_t value_;
  uint64_t range_;
  uint64_t divisor_;
};


#endif // SRC_LINE_PART
