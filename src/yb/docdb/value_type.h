// Copyright (c) YugaByte, Inc.

#ifndef YB_DOCDB_VALUE_TYPE_H_
#define YB_DOCDB_VALUE_TYPE_H_

#include <string>

#include <glog/logging.h>

#include "rocksdb/slice.h"

namespace yb {
namespace docdb {

// Changes to this enum may invalidate persistent data.
enum class ValueType : char {
  // This indicates the end of the "hashed" or "range" group of components of the primary key. This
  // needs to sort before all other value types, so that a DocKey that has a prefix of the sequence
  // of components of another key sorts before the other key.
  kGroupEnd = '!',  // ASCII code 33 -- we pick the lowest code graphic character.

  // HybridTime must be lower than all other primitive types (other than kGroupEnd) so that
  // SubDocKeys that have fewer subkeys within a document sort above those that have all the same
  // subkeys and more. In our MVCC document model layout the hybrid time always appears at the end
  // of the key.
  kHybridTime = '#',  // ASCII code 35 (34 is double quote, which would be a bit confusing here).

  // Primitive value types
  kString = '$',  // ASCII code 36
  kArray = 'A',  // ASCII code 65. TODO: do we need this at the this layer?
  kDouble = 'D',  // ASCII code 68
  kFalse = 'F',  // ASCII code 70
  kUInt16Hash = 'G',  // ASCII code 71
  kInt64 = 'I',  // ASCII code 73
  kSystemColumnId = 'J',  // ASCII code 74
  kColumnId = 'K',  // ASCII code 75
  kNull = 'N',  // ASCII code 78
  kTrue = 'T',  // ASCII code 84
  kTombstone = 'X',  // ASCII code 88
  kArrayIndex = '[',  // ASCII code 91. TODO: do we need this at this layer?

  // We allow putting a 32-bit hash in front of the document key. This hash is computed based on
  // the "hashed" components of the document key that precede "range" components.

  kStringDescending = 'a',  // ASCII code 97
  kInt64Descending = 'b',  // ASCII code 98
  kTimestampDescending = 'c',  // ASCII code 99

  // Timestamp value in microseconds
  kTimestamp = 's',  // ASCII code 115
  // TTL value in milliseconds, optionally present at the start of a value.
  kTtl = 't',  // ASCII code 116

  kObject = '{',  // ASCII code 123

  // This is used for sanity checking.
  kInvalidValueType = 127
};

// All primitive value types fall into this range, but not all value types in this range are
// primitive (e.g. object and tombstone are not).

constexpr ValueType kMinPrimitiveValueType = ValueType::kString;
constexpr ValueType kMaxPrimitiveValueType = ValueType::kObject;

std::string ValueTypeToStr(ValueType value_type);

constexpr inline bool IsPrimitiveValueType(const ValueType value_type) {
  return kMinPrimitiveValueType <= value_type && value_type <= kMaxPrimitiveValueType &&
         value_type != ValueType::kObject &&
         value_type != ValueType::kArrayIndex &&
         value_type != ValueType::kTombstone;
}

inline ValueType DecodeValueType(const rocksdb::Slice& value) {
  CHECK(!value.empty());  // TODO: graceful error handling.
  return static_cast<ValueType>(value.data()[0]);
}

inline ValueType ConsumeValueType(rocksdb::Slice* slice) {
  CHECK(!slice->empty());  // TODO: graceful error handling.
  return static_cast<ValueType>(slice->ConsumeByte());
}

inline std::ostream& operator<<(std::ostream& out, const ValueType value_type) {
  return out << ValueTypeToStr(value_type);
}

}  // namespace docdb
}  // namespace yb

#endif  // YB_DOCDB_VALUE_TYPE_H_
