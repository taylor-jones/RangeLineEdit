#include "line_part.h"

namespace {

int64_t safe_add(int64_t a, int64_t b) {
  if (a > 0 && b > LLONG_MAX - a) return LLONG_MAX;
  if (a < 0 && b < LLONG_MIN - a) return LLONG_MIN;
  return a + b;
}

template <typename T>
T* safe_cast(LinePart* part) {
  if (part == nullptr) return nullptr;
  return dynamic_cast<T*>(part);
}

LinePart* head_part(LinePart* part) {
  LinePart* head = part;
  while (head != nullptr && head->prevPart() != nullptr) {
    head = head->prevPart();
  }
  return head;
}

template <typename T>
T* head_part(LinePart* part) {
  T* head = dynamic_cast<T*>(part);
  while (head != nullptr && dynamic_cast<T*>(head->prevPart()) != nullptr) {
    head = static_cast<T*>(head->prevPart());
  }
  return head;
}

bool preceded_by_all_zeros(LinePart* part) {
  if (part == nullptr) return false;

  bool all_zeros = true;
  auto* prev = part->prevPart();
  while (prev != nullptr) {
    auto* prev_int = dynamic_cast<Int64LinePart*>(prev);
    if (prev_int != nullptr) all_zeros &= (prev_int->value() == 0LL);
    prev = prev->prevPart();
  }
  return all_zeros;
}

bool first_char_is_positive(LinePart* part) {
  auto* first_char_part = head_part<CharLinePart>(part);
  return first_char_part == nullptr || first_char_part->value() == first_char_part->positiveChar();
}

}  // namesspace


//
// Int64LinePart
//

Int64LinePart::Int64LinePart(uint64_t range, int64_t divisor) {
  setRange(range);
  setDivisor(divisor);
}

void Int64LinePart::setRange(uint64_t range) {
  range_ = range;
  if (value_ > range_) value_ = range_;
}

void Int64LinePart::setDivisor(uint64_t divisor) {
  divisor_ = divisor;
}

void Int64LinePart::setValue(int64_t value) {
  value_ = value;
  if (value > 0LL && value > range_) value_ = range_;
  if (value <= 0LL && value < -range_) value_ = -range_;
}

bool Int64LinePart::setValueAtIndex(const QChar& value, int index) {
  if (QRegExp("\\d").exactMatch(value)) {
    bool ok;
    const int64_t coefficient = (value_ >= 0LL ? 1LL : -1LL);
    const int64_t candidate_value = valueString().replace(index, 1, value).toLongLong(&ok) * coefficient;

    if (ok && llabs(candidate_value) <= range_) {
      value_ = candidate_value;
      return true;
    }
  }
  return false;
}


bool Int64LinePart::increment(int index) {
  // Note: The index refers to the significant figure index (right-to-left indexing).
  // This determines if we're at the "ones", "tens", "hundreds", etc. place to determine
  // what this widget will increment by.
  int64_t value_to_increment_by = std::pow(10LL, index);
  int64_t prev_value = value_;
  auto* prev_int_part = safe_cast<Int64LinePart>(prevPart());

  auto candidate_value = safe_add(value_, value_to_increment_by);

  // Incrementing a positive number...
  if (value_ > 0LL) {
    if (candidate_value > range_) {
      if (prev_int_part != nullptr && prev_int_part->increment(0)) {
        setValue(value_ + (value_to_increment_by - range_ - 1LL));
      } else {
        setValue(range_);
      }
    } else {
      setValue(candidate_value);
    }

  // Incrementing a negative number...
  } else if (value_ < 0LL) {
    if (candidate_value > 0LL) {
      auto* first_int_part = head_part<Int64LinePart>(this);
      bool changing_from_neg_to_pos = preceded_by_all_zeros(this)
        && first_int_part != nullptr
        && first_int_part->increment(0);
      if (changing_from_neg_to_pos) {
        setValue(-value_);
      } else if (prev_int_part != nullptr && prev_int_part->increment(0)) {
        setValue(-range_ + (value_to_increment_by + value_ - 1LL));
      } else {
        setValue(candidate_value);
      }
    } else {
      setValue(candidate_value);
    }

  // Incrementing a zero number...
  } else if (value_ == 0LL) {
    if (first_char_is_positive(this) && candidate_value < range_) {
      auto* first_int_part = head_part<Int64LinePart>(this);
      if (first_int_part != nullptr && first_int_part->increment(0)) {
        setValue(candidate_value);
      }
    } else {
      if (prev_int_part != nullptr && prev_int_part->increment(0)) {
        setValue(-range_ - 1LL + value_to_increment_by);
      } else {
        setValue(-range_);
      }
    }
  }

  return value_ != prev_value;
}


bool Int64LinePart::decrement(int index) {
  return true;
}
