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
// Thread-safe logging routines that do not allocate any memory or
// acquire any locks, and can therefore be used by low-level memory
// allocation, synchronization, and signal-handling code.
//
// NOTE FOR GOOGLERS:
//
// IWYU pragma: private, include "base/raw_logging.h"

#ifndef S2_THIRD_PARTY_XBSL_BASE_INTERNAL_RAW_LOGGING_H_
#define S2_THIRD_PARTY_XBSL_BASE_INTERNAL_RAW_LOGGING_H_

#include "s2/third_party/xbsl/base/attributes.h"
#include "s2/third_party/xbsl/base/log_severity.h"
#include "s2/third_party/xbsl/base/macros.h"
#include "s2/third_party/xbsl/base/port.h"

// This is similar to LOG(severity) << format..., but
// * it is to be used ONLY by low-level modules that can't use normal LOG()
// * it is designed to be a low-level logger that does not allocate any
//   memory and does not need any locks, hence:
// * it logs straight and ONLY to STDERR w/o buffering
// * it uses an explicit printf-format and arguments list
// * it will silently chop off really long message strings
// Usage example:
//   XBSL_RAW_LOG(ERROR, "Failed foo with %i: %s", status, error);
// This will print an almost standard log line like this to stderr only:
//   E0821 211317 file.cc:123] RAW: Failed foo with 22: bad_file

#if !defined(STRIP_LOG) || STRIP_LOG == 0
#define XBSL_RAW_LOG(severity, ...)                                            \
  do {                                                                         \
    constexpr const char* xbsl_raw_logging_internal_basename =                 \
        ::xbsl::raw_logging_internal::Basename(__FILE__,                       \
                                               sizeof(__FILE__) - 1);          \
    ::xbsl::raw_logging_internal::RawLog(XBSL_RAW_LOGGING_INTERNAL_##severity, \
                                         xbsl_raw_logging_internal_basename,   \
                                         __LINE__, __VA_ARGS__);               \
  } while (0)
#else
#define XBSL_RAW_LOG(severity, ...)                                            \
  do {                                                                         \
    constexpr const char* xbsl_raw_logging_internal_basename =                 \
        ::xbsl::raw_logging_internal::Basename(__FILE__,                       \
                                               sizeof(__FILE__) - 1);          \
    if (XBSL_RAW_LOGGING_INTERNAL_##severity == ::xbsl::LogSeverity::kFatal)   \
      ::xbsl::raw_logging_internal::RawLog(::xbsl::LogSeverity::kFatal,        \
                                           xbsl_raw_logging_internal_basename, \
                                           __LINE__, __VA_ARGS__);             \
  } while (0)
#endif

// Similar to CHECK(condition) << message, but for low-level modules:
// we use only XBSL_RAW_LOG that does not allocate memory.
// We do not want to provide args list here to encourage this usage:
//   if (!cond)  XBSL_RAW_LOG(FATAL, "foo ...", hard_to_compute_args);
// so that the args are not computed when not needed.
#define XBSL_RAW_CHECK(condition, message)                             \
  do {                                                                 \
    if (XBSL_PREDICT_FALSE(!(condition))) {                            \
      XBSL_RAW_LOG(FATAL, "Check %s failed: %s", #condition, message); \
    }                                                                  \
  } while (0)

#ifndef NDEBUG

#define RAW_DLOG(severity, ...) XBSL_RAW_LOG(severity, __VA_ARGS__)
#define RAW_DCHECK(condition, message) XBSL_RAW_CHECK(condition, message)

#else  // NDEBUG

#define RAW_DLOG(severity, ...) \
  while (false) XBSL_RAW_LOG(severity, __VA_ARGS__)
#define RAW_DCHECK(condition, message) \
  while (false) XBSL_RAW_CHECK(condition, message)

#endif  // NDEBUG

#define XBSL_RAW_LOGGING_INTERNAL_INFO ::xbsl::LogSeverity::kInfo
#define XBSL_RAW_LOGGING_INTERNAL_WARNING ::xbsl::LogSeverity::kWarning
#define XBSL_RAW_LOGGING_INTERNAL_ERROR ::xbsl::LogSeverity::kError
#define XBSL_RAW_LOGGING_INTERNAL_FATAL ::xbsl::LogSeverity::kFatal
#define XBSL_RAW_LOGGING_INTERNAL_DFATAL ::xbsl::kLogDebugFatal
#define XBSL_RAW_LOGGING_INTERNAL_LEVEL(severity) \
  ::xbsl::NormalizeLogSeverity(severity)

namespace xbsl {
namespace raw_logging_internal {

// Helper function to implement XBSL_RAW_LOG
// Logs format... at "severity" level, reporting it
// as called from file:line.
// This does not allocate memory or acquire locks.
void RawLog(xbsl::LogSeverity severity, const char* file, int line,
            const char* format, ...) XBSL_PRINTF_ATTRIBUTE(4, 5);
void RawLog(int severity, const char* file, int line, const char* format, ...)
    XBSL_PRINTF_ATTRIBUTE(4, 5);

// Writes the provided buffer directly to stderr, in a safe, low-level manner.
//
// In POSIX this means calling write(), which is async-signal safe and does
// not malloc.  If the platform supports the SYS_write syscall, we invoke that
// directly to side-step any libc interception.
void SafeWriteToStderr(const char *s, size_t len);

// compile-time function to get the "base" filename, that is, the part of
// a filename after the last "/" or "\" path separator.  The search starts at
// the end of the string; the second parameter is the length of the string.
constexpr const char* Basename(const char* fname, int offset) {
  return offset == 0 || fname[offset - 1] == '/' || fname[offset - 1] == '\\'
             ? fname + offset
             : Basename(fname, offset - 1);
}

// For testing only.
// Returns true if raw logging is fully supported. When it is not
// fully supported, no messages will be emitted, but a log at FATAL
// severity will cause an abort.
//
// TODO(user): Come up with a better name for this method.
bool RawLoggingFullySupported();

// Function type for a raw_logging customization hook for suppressing messages
// by severity, and for writing custom prefixes on non-suppressed messages.
//
// The installed hook is called for every raw log invocation.  The message will
// be logged to stderr only if the hook returns true.  FATAL errors will cause
// the process to abort, even if writing to stderr is suppressed.  The hook is
// also provided with an output buffer, where it can write a custom log message
// prefix.
//
// The raw_logging system does not allocate memory or grab locks.  User-provided
// hooks must avoid these operations, and must not throw exceptions.
//
// 'severity' is the severity level of the message being written.
// 'file' and 'line' are the file and line number where the XBSL_RAW_LOG macro
// was located.
// 'buffer' and 'buf_size' are pointers to the buffer and buffer size.  If the
// hook writes a prefix, it must increment *buffer and decrement *buf_size
// accordingly.
using LogPrefixHook = bool (*)(xbsl::LogSeverity severity, const char* file,
                               int line, char** buffer, int* buf_size);

// Function type for a raw_logging customization hook called to abort a process
// when a FATAL message is logged.  If the provided AbortHook() returns, the
// logging system will call abort().
//
// 'file' and 'line' are the file and line number where the XBSL_RAW_LOG macro
// was located.
// The null-terminated logged message lives in the buffer between 'buf_start'
// and 'buf_end'.  'prefix_end' points to the first non-prefix character of the
// buffer (as written by the LogPrefixHook.)
using AbortHook = void (*)(const char* file, int line, const char* buf_start,
                           const char* prefix_end, const char* buf_end);

}  // namespace raw_logging_internal
}  // namespace xbsl


// TODO(b/62299050): These will be removed in post-launch cleanup
#define RAW_LOG XBSL_RAW_LOG
#define RAW_CHECK XBSL_RAW_CHECK

namespace base_raw_logging {


XBSL_DEPRECATED("Use xbsl::raw_logging_internal::RawLog instead.")
void RawLog(xbsl::LogSeverity severity, const char* file, int line,
            const char* format, ...) XBSL_PRINTF_ATTRIBUTE(4, 5);
XBSL_DEPRECATED("Use xbsl::raw_logging_internal::RawLog instead.")
void RawLog(int severity, const char* file, int line, const char* format, ...)
    XBSL_PRINTF_ATTRIBUTE(4, 5);

// Since raw_logging is internal in xbsl, the below functions and types
// won't need to be shipped yet.

using LogPrefixHook = xbsl::raw_logging_internal::LogPrefixHook;
using AbortHook = xbsl::raw_logging_internal::AbortHook;

// Registers hooks of the above types.  Only a single hook of each type may be
// registered.  It is an error to call these functions multiple times with
// different input arguments.
//
// These functions are safe to call at any point during initialization; they do
// not block or malloc, and are async-signal safe.
void RegisterLogPrefixHook(LogPrefixHook fn);
void RegisterAbortHook(AbortHook fn);

}  // namespace base_raw_logging


#endif  // S2_THIRD_PARTY_XBSL_BASE_INTERNAL_RAW_LOGGING_H_
