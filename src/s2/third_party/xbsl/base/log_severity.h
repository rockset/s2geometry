// Copyright 2017 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//
// NOTE FOR GOOGLERS:
//
// IWYU pragma: private, include "base/log_severity.h"

#ifndef S2_THIRD_PARTY_XBSL_BASE_INTERNAL_LOG_SEVERITY_H_
#define S2_THIRD_PARTY_XBSL_BASE_INTERNAL_LOG_SEVERITY_H_

#include <array>

#include "s2/third_party/xbsl/base/attributes.h"

namespace xbsl {

// Four severity levels are defined.  Logging APIs should terminate the program
// when a message is logged at severity `kFatal`; the other levels have no
// special semantics.
enum class LogSeverity : int {
  kInfo = 0,
  kWarning = 1,
  kError = 2,
  kFatal = 3,
};

// Returns an iterable of all standard `xbsl::LogSeverity` values, ordered from
// least to most severe.
constexpr std::array<xbsl::LogSeverity, 4> LogSeverities() {
  return {{xbsl::LogSeverity::kInfo, xbsl::LogSeverity::kWarning,
           xbsl::LogSeverity::kError, xbsl::LogSeverity::kFatal}};
}

// `xbsl::kLogDebugFatal` equals `xbsl::LogSeverity::kFatal` in debug builds
// (i.e. when `NDEBUG` is not defined) and `xbsl::LogSeverity::kError`
// otherwise.  It is extern to prevent ODR violations when compilation units
// with different build settings are linked together.
XBSL_CONST_INIT extern const xbsl::LogSeverity kLogDebugFatal;

// Returns the all-caps string representation (e.g. "INFO") of the specified
// severity level if it is one of the normal levels and "UNKNOWN" otherwise.
constexpr const char* LogSeverityName(xbsl::LogSeverity s) {
  return s == xbsl::LogSeverity::kInfo
             ? "INFO"
             : s == xbsl::LogSeverity::kWarning
                   ? "WARNING"
                   : s == xbsl::LogSeverity::kError
                         ? "ERROR"
                         : s == xbsl::LogSeverity::kFatal ? "FATAL" : "UNKNOWN";
}

// Values less than `kInfo` normalize to `kInfo`; values greater than `kFatal`
// normalize to `kError` (**NOT** `kFatal`).
constexpr xbsl::LogSeverity NormalizeLogSeverity(xbsl::LogSeverity s) {
  return s < xbsl::LogSeverity::kInfo
             ? xbsl::LogSeverity::kInfo
             : s > xbsl::LogSeverity::kFatal ? xbsl::LogSeverity::kError : s;
}
constexpr xbsl::LogSeverity NormalizeLogSeverity(int s) {
  return NormalizeLogSeverity(static_cast<xbsl::LogSeverity>(s));
}

}  // namespace xbsl

#endif  // S2_THIRD_PARTY_XBSL_BASE_INTERNAL_LOG_SEVERITY_H_
