#include "Ranges.h"

#include <QRegExp>
#include <QDebug>
#include <cmath>

// ---------------------------------------------------------------------------
// Range definitions
// ---------------------------------------------------------------------------

Range::Range() 
  : m_charIndexStart(0),
    m_charIndexEnd(0),
    m_leftRange(nullptr),
    m_rightRange(nullptr),
    m_dirty(false) {}


Range::~Range() {
  m_leftRange = nullptr;
  m_rightRange = nullptr;
}


Range* Range::leftMostRange() {
  Range* range = m_leftRange;
  while (range != nullptr && range->m_leftRange != nullptr) {
    range = range->m_leftRange;
  }
  return range;
}


RangeInt* Range::leftMostRangeInt() {
  RangeInt* rangeInt = dynamic_cast<RangeInt*>(this);
  Range* rangeIter = this->m_leftRange;
  while (rangeIter != nullptr) {
    // Note: We don't break because we might find another, more left RangeInt
    if (rangeIter->rangeType() == "RangeInt") {
      rangeInt = static_cast<RangeInt*>(rangeIter);
    }
    rangeIter = rangeIter->m_leftRange;
  }

  return rangeInt;
}


bool Range::allValuesToLeftAreZero() {
  // Assume true and prove otherwise
  bool allRangeIntsToLeftAreZero(true);
  Range* rangeIter = this->m_leftRange;
  while (rangeIter != nullptr) {
    // Note: We don't break because we might find another, more left RangeInt
    if (rangeIter->rangeType() == "RangeInt") {
      allRangeIntsToLeftAreZero &= (static_cast<RangeInt*>(rangeIter)->m_value == 0LL);
    }
    rangeIter = rangeIter->m_leftRange;
  }
  return allRangeIntsToLeftAreZero;
}


bool Range::leftMostRangeCharSign() {
  // Assume true and prove otherwise
  bool positiveSign(true);
  Range* rangeIter = this->m_leftRange;
  while (rangeIter != nullptr) {
    if (rangeIter->rangeType() == "RangeChar") {
      positiveSign = (static_cast<RangeChar*>(rangeIter)->m_value == static_cast<RangeChar*>(rangeIter)->m_positiveChar);
    }
    rangeIter = rangeIter->m_leftRange;
  }
  return positiveSign;
}


// ---------------------------------------------------------------------------
// RangeChar definitions
// ---------------------------------------------------------------------------

RangeChar::RangeChar(QChar negativeChar, QChar positiveChar)
    : Range(),
      m_negativeChar(negativeChar),
      m_positiveChar(positiveChar),
      m_value(m_positiveChar) {}

void RangeChar::setRange(QChar negativeChar, QChar positiveChar) {
  m_negativeChar = negativeChar;
  m_positiveChar = positiveChar;
}

bool RangeChar::increment(int) {
  bool incremented(false);
  if (m_value != m_positiveChar) {
    incremented = true;
    m_value = m_positiveChar;
    m_dirty = true;
  }
  return incremented;
}

bool RangeChar::decrement(int) {
  bool decremented(false);
  if (m_value != m_negativeChar) {
    m_value = m_negativeChar;
    decremented = true;
    m_dirty = true;
  }
  return decremented;
}

int RangeChar::valueLength() {
  return 1;
}

int RangeChar::rangeLength() {
  return 1;
}

QString RangeChar::valueStr() {
  return QString(m_value);
}

QString RangeChar::rangeType() {
  return "RangeChar";
}

long long RangeChar::divisor() {
  return 1LL;
}

bool RangeChar::setValueForIndex(const QChar& value, int index) {
  bool valueWasSet = false;
  // Lazily evaluate
  if ((index == m_charIndexEnd) && (value.toLower() != m_value.toLower())) {
    // Check case insensitively
    if ((value.toLower() == m_positiveChar.toLower()) || (value.toLower() == m_negativeChar.toLower())) {
      m_value = m_value.isUpper() ? value.toUpper() : value.toLower();
      m_dirty = true;
      valueWasSet = true;
    }
  }
  return valueWasSet;
}


// ---------------------------------------------------------------------------
// RangeStringConstant definitions
// ---------------------------------------------------------------------------

RangeStringConstant::RangeStringConstant(const QString& stringPlaceHolder) : Range(), m_value(stringPlaceHolder) {}


bool RangeStringConstant::increment(int) {
  bool incremented(true);
  if (m_leftRange != nullptr) {
    incremented = m_leftRange->increment(0);
  }
  return incremented;
}


bool RangeStringConstant::decrement(int) {
  bool decremented(true);
  if (m_leftRange != nullptr) {
    decremented = m_leftRange->decrement(0);
  }
  return decremented;
}


int RangeStringConstant::valueLength() {
  return m_value.length();
}

int RangeStringConstant::rangeLength() {
  return m_value.length();
}

QString RangeStringConstant::valueStr() {
  return m_value;
}

QString RangeStringConstant::rangeType() {
  return "RangeStringConstant";
}

long long RangeStringConstant::divisor() {
  return 1LL;
}

bool RangeStringConstant::setValueForIndex(const QChar&, int) {
  return true;
}


// ---------------------------------------------------------------------------
// RangeInt definitions
// ---------------------------------------------------------------------------

RangeInt::RangeInt(long long range, long long divisor, bool carryOrBorrowFromLeft, bool isSigned)
    : Range(),
      m_range(1LL),
      m_value(0LL),
      m_divisor(divisor),
      m_carryOrBorrowFromLeft(carryOrBorrowFromLeft),
      m_signed(isSigned) {
  setRange(range);
}


bool RangeInt::setRange(long long range) {
  if (range <= 0LL) {
    qCritical().nospace() << __FUNCTION__ << " | range must be > 0 (given:" << range << ")";
    return false;
  }

  m_range = range;
  if (m_value > m_range) {
    m_value = m_range;
    m_dirty = true;
  }
  return true;
}


bool RangeInt::setDivisor(long long divisor) {
  if (divisor <= 0LL) {
    qCritical().nospace() << __FUNCTION__ << " | divisor must be > 0 (given:" << divisor << ")";
    return false;
  }

  m_divisor = divisor;
  return true;
}


void RangeInt::setValue(long long value) {
  m_value = value;
  // If it's unsigned and less than 0, then 0 it out
  if (m_signed == false && m_value < 0LL) {
    m_value = 0LL;
  }

  if (m_value >= 0LL && m_value > m_range) {
    m_value = m_range;
  } else if (m_value <= 0LL && m_value < -m_range) {
    m_value = -m_range;
  }
}


long long safeAdd(long long a, long long b) {
  // long long retVal = 0LL;
  if (a > 0 && b > LLONG_MAX - a) return LLONG_MAX;
  if (a < 0 && b < LLONG_MIN - a) return LLONG_MIN;
  return a + b;
}


bool RangeInt::increment(int index) {
  // Note: The index refers to the significant figure index (right to left indexing)
  // This determines if we're at the "ones", "tens", "hundreds", etc. place to determine
  // what this widget will increment by
  long long valueToIncrementBy = std::pow(10LL, index);
  long long originalValue = m_value;

  // Incrementing a positive number (Should make the number diverge from 0 (i.e. 20 + 10 = 30)
  if (m_value > 0LL) {
    if (safeAdd(m_value, valueToIncrementBy) > m_range) {
      if (m_leftRange != nullptr && m_carryOrBorrowFromLeft) {
        // If we can increment our left Range
        if (m_leftRange->increment(0)) {
          setValue(m_value + (valueToIncrementBy - m_range - 1LL));
        } else {
          // We've hit our maximum value if we couldn't increment recursively
          setValue(m_range);
        }
      } else {
        // We've hit our maximum value if we couldn't increment recursively
        setValue(m_range);
      }
    } else {
      // Else we don't have an addition carry, so we can just normally add and not affect our neighboring left range
      setValue(m_value + valueToIncrementBy);
    }
  }
  // Incrementing a negative number (Should make the number approach 0 (i.e. -20 + 10 = -10)
  else if (m_value < 0LL) {
    // If we increment our value above 0, but we're supposed to be negative (i.e. -05 + 10 = -55 && increment the left)
    if (safeAdd(m_value, valueToIncrementBy) > 0LL) {
      // If all values to our left are zeroed out, but we can flip the RangeChar (i.e. S -> N or W -> E),
      // then our addition carry will be satisfied by the RangeChar's incrementation
      bool flippingFromNegToPos = allValuesToLeftAreZero() && leftMostRange() != nullptr && leftMostRange()->increment(0);
      if (flippingFromNegToPos) {
        setValue(-m_value);
      // If not everything to the left is zeroed out, that means we should forward the carry bit to our left range
      // (i.e. S01 05' -> S00 55')
      } else if (m_carryOrBorrowFromLeft && m_leftRange != nullptr && m_leftRange->increment(0)) {
        setValue(-m_range + (valueToIncrementBy + m_value - 1LL));
      // If there's no left ranges at all, attempt to treat this as a stand alone value that can be made positive
      } else {
        setValue(m_value + valueToIncrementBy);
      }
      // Else we can just increment because we're still going to be negative (i.e. -15 + 10 = -05)
    } else {
      setValue(m_value + valueToIncrementBy);
    }
  }
  // This is a special case, depending on if our left-most RangeChar is positive or not
  else {
    // If the whole value is positive, we can safely increment
    if (leftMostRangeCharSign() == true && safeAdd(m_value, valueToIncrementBy) < m_range) {
      setValue(m_value + valueToIncrementBy);
    }
    // If the whole value is negative (Or we're exceeding our range), we need to check if everything to our left is
    // zeroed out or not
    else {
      if (allValuesToLeftAreZero() && (safeAdd(m_value, valueToIncrementBy) < m_range)) {
        // Should increment the RangeChar from its negative sign to its positive sign
        if (leftMostRange() != nullptr && leftMostRange()->increment(0)) {
          setValue(m_value + valueToIncrementBy);
        }
      } else {
        // We're actually incrementing a negative left neighboring range instead
        if (m_carryOrBorrowFromLeft && m_leftRange != nullptr && m_leftRange->increment(0)) {
          setValue(-m_range - 1LL + valueToIncrementBy);
        } else {
          setValue(-m_range);
        }
      }
    }
  }

  m_dirty = (m_value != originalValue);
  return m_dirty;
}


bool RangeInt::decrement(int index) {
  // Note: The index refers to the significant figure index (right to left indexing)
  // This determines if we're at the "ones", "tens", "hundreds", etc. place to determine
  // what this widget will decrement by
  long long valueToDecrementBy = std::pow(10LL, index);
  long long originalValue = m_value;
  // Decrementing a positive number (i.e. 20 - 10 = 10)
  if (m_value > 0LL) {
    // We're borrowing a negative from our left (if all non-zero) or the rangeChar
    if (m_carryOrBorrowFromLeft && (safeAdd(m_value, -valueToDecrementBy) < 0LL)) {
      bool flippingFromPosToNeg = allValuesToLeftAreZero() && leftMostRange() != nullptr && leftMostRange()->decrement(0);

      if (flippingFromPosToNeg) {
        setValue(-m_value);
      } else if (m_leftRange != nullptr && m_leftRange->decrement(0)) {
        //(i.e. N01 05' - 10' -> N00 55')
        setValue(m_range - (llabs(valueToDecrementBy - m_value) - 1LL));
      } else {
        setValue(m_value - valueToDecrementBy);
      }
    } else {
      setValue(safeAdd(m_value, -valueToDecrementBy));
    }
  }
  // Decrementing a negative number (i.e. -20 - 10 = -30)
  else if (m_value < 0LL) {
    // If we underflow (i.e. -55 - 10 = -05 IF we can borrow from our left)
    if (safeAdd(m_value, -valueToDecrementBy) < -m_range) {
      // If we can decrement the left range
      if (m_carryOrBorrowFromLeft && m_leftRange != nullptr && m_leftRange->decrement(0)) {
        //(i.e. -55 - 10 + 59 + 1 = -65 + 60 = -5)
        setValue(m_value - valueToDecrementBy + m_range + 1LL);
      } else {
        setValue(-m_range);
      }
    } else {
      //(i.e. -45 - 10 = -55)
      setValue(safeAdd(m_value, -valueToDecrementBy));
    }
  }
  // This is a special case, depending on if our left neighbor is the RangeChar or not
  else {
    if (leftMostRangeCharSign() == true) {
      if (allValuesToLeftAreZero() == true) {
        // Will attempt to flip the 'N' -> 'S' and borrow from the sign to allow us to become negative
        if (leftMostRange() != nullptr) {
          leftMostRange()->decrement(0);
        }
        // If there is no RangeChar, then let us do it anyway and assume -m_range is our limit
        setValue(safeAdd(m_value, -valueToDecrementBy));
      }
      // Else not everything to our left is negative and we should be able to borrow from our left
      else {
        if (m_carryOrBorrowFromLeft && m_leftRange != nullptr && m_leftRange->decrement(0)) {
          //(i.e. N01 00' - 10' -> N00 50')
          setValue(m_range - (llabs(valueToDecrementBy - m_value) - 1LL));
        } else {
          setValue(safeAdd(m_value, -valueToDecrementBy));
        }
      }
    }
    // Else we're negative already, so we should be able to just subtract
    else {
      setValue(safeAdd(m_value, -valueToDecrementBy));
    }
  }

  m_dirty = (m_value != originalValue);
  return m_dirty;
}


int RangeInt::valueLength() {
  return QString::number(llabs(m_value)).length();
}

int RangeInt::rangeLength() {
  return QString::number(m_range).length();
}

QString RangeInt::valueStr() {
  return QString("0").repeated(rangeLength() - valueLength()) + QString::number(llabs(m_value));
}

QString RangeInt::rangeType() {
  return "RangeInt";
}

long long RangeInt::divisor() {
  return m_divisor;
}

bool RangeInt::setValueForIndex(const QChar& value, int index) {
  bool valueWasSet = false;
  // This allows single digit replacement while getting keyboard input from the user.
  // For example, Degrees goes from [000, 180]
  // We attempt to replace the value for index string and convert the value back to an int and then verify it's within
  // range. (i.e. value = 7 && index == 1, -> [070]
  if (QRegExp("\\d").exactMatch(value)) {
    bool success(false);
    long long attemptedValue = valueStr().replace(index, 1, value).toLongLong(&success) * (m_value >= 0LL ? 1LL : -1LL);

    if (success && llabs(attemptedValue) <= m_range) {
      m_value = attemptedValue;
      m_dirty = true;
      valueWasSet = true;
    }
  }

  return valueWasSet;
}
