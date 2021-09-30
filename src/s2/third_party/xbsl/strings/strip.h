//
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
// -----------------------------------------------------------------------------
// File: strip.h
// -----------------------------------------------------------------------------
//
// This file contains various functions for stripping substrings from a string.
#ifndef S2_THIRD_PARTY_XBSL_STRINGS_STRIP_H_
#define S2_THIRD_PARTY_XBSL_STRINGS_STRIP_H_

#include <cstddef>
#include <string>

#include "s2/third_party/xbsl/base/macros.h"
#include "s2/third_party/xbsl/strings/ascii.h"
#include "s2/third_party/xbsl/strings/ascii_ctype.h"
#include "s2/third_party/xbsl/strings/match.h"
#include "s2/third_party/xbsl/strings/string_view.h"

namespace xbsl {

// ConsumePrefix()
//
// Strips the `expected` prefix from the start of the given string, returning
// `true` if the strip operation succeeded or false otherwise.
//
// Example:
//
//   xbsl::string_view input("abc");
//   EXPECT_TRUE(xbsl::ConsumePrefix(&input, "a"));
//   EXPECT_EQ(input, "bc");
inline bool ConsumePrefix(xbsl::string_view* str, xbsl::string_view expected) {
  if (!xbsl::StartsWith(*str, expected)) return false;
  str->remove_prefix(expected.size());
  return true;
}
// ConsumeSuffix()
//
// Strips the `expected` suffix from the end of the given string, returning
// `true` if the strip operation succeeded or false otherwise.
//
// Example:
//
//   xbsl::string_view input("abcdef");
//   EXPECT_TRUE(xbsl::ConsumeSuffix(&input, "def"));
//   EXPECT_EQ(input, "abc");
inline bool ConsumeSuffix(xbsl::string_view* str, xbsl::string_view expected) {
  if (!xbsl::EndsWith(*str, expected)) return false;
  str->remove_suffix(expected.size());
  return true;
}

// StripPrefix()
//
// Returns a view into the input string 'str' with the given 'prefix' removed,
// but leaving the original string intact. If the prefix does not match at the
// start of the string, returns the original string instead.
XBSL_MUST_USE_RESULT inline xbsl::string_view StripPrefix(
    xbsl::string_view str, xbsl::string_view prefix) {
  if (xbsl::StartsWith(str, prefix)) str.remove_prefix(prefix.size());
  return str;
}

// StripSuffix()
//
// Returns a view into the input string 'str' with the given 'suffix' removed,
// but leaving the original string intact. If the suffix does not match at the
// end of the string, returns the original string instead.
XBSL_MUST_USE_RESULT inline xbsl::string_view StripSuffix(
    xbsl::string_view str, xbsl::string_view suffix) {
  if (xbsl::EndsWith(str, suffix)) str.remove_suffix(suffix.size());
  return str;
}

}  // namespace xbsl


// Replaces any of the *bytes* in 'remove' with the *byte* 'replace_with'.
//
// *Warning*: This function operates on *bytes* in the remove string.
// When the remove string contains multi-byte (non-ASCII) characters,
// then some strings will turn into garbage which will break downstream code.
// Use icu::UnicodeSet and its spanUTF8()/spanBackUTF8().
void ReplaceCharacters(char* str, size_t len, xbsl::string_view remove,
                       char replace_with);
void ReplaceCharacters(string* s, xbsl::string_view remove, char replace_with);

// Replaces the character 'remove' with the character 'replace_with'.
//
inline void ReplaceCharacter(char* str, size_t len, char remove,
                             char replace_with) {
  for (char* end = str + len; str != end; ++str) {
    if (*str == remove) *str = replace_with;
  }
}

XBSL_DEPRECATED("Use xbsl::StripAsciiWhitespace() instead")
inline void StripWhitespace(string* str) { xbsl::StripAsciiWhitespace(str); }

XBSL_DEPRECATED("Use xbsl::StripAsciiWhitespace() instead")
inline void StripWhitespace(xbsl::string_view* str) {
  *str = xbsl::StripAsciiWhitespace(*str);
}

// Returns a pointer to the first character in 'str' that is not
// ASCII whitespace. Never returns nullptr. 'str' must be NUL-terminated.
inline const char* SkipLeadingWhitespace(const char* str) {
  while (xbsl::ascii_isspace(*str)) ++str;
  return str;
}
inline char* SkipLeadingWhitespace(char* str) {
  while (xbsl::ascii_isspace(*str)) ++str;
  return str;
}


#endif  // S2_THIRD_PARTY_XBSL_STRINGS_STRIP_H_
