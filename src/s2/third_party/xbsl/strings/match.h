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
// File: match.h
// -----------------------------------------------------------------------------
//
// This file contains simple utilities for performing string matching checks.
// All of these function parameters are specified as `xbsl::string_view`,
// meaning that these functions can accept `std::string`, `xbsl::string_view` or
// nul-terminated C-style strings.
//
// Examples:
//   std::string s = "foo";
//   xbsl::string_view sv = "f";
//   assert(xbsl::StrContains(s, sv));
//
// Note: The order of parameters in these functions is designed to mimic the
// order an equivalent member function would exhibit;
// e.g. `s.Contains(x)` ==> `xbsl::StrContains(s, x).
#ifndef S2_THIRD_PARTY_XBSL_STRINGS_MATCH_H_
#define S2_THIRD_PARTY_XBSL_STRINGS_MATCH_H_

#include <cstring>

#include "s2/third_party/xbsl/strings/string_view.h"

namespace xbsl {

// StrContains()
//
// Returns whether a given string `haystack` contains the substring `needle`.
inline bool StrContains(xbsl::string_view haystack, xbsl::string_view needle) {
  return haystack.find(needle, 0) != haystack.npos;
}

// StartsWith()
//
// Returns whether a given string `text` begins with `prefix`.
inline bool StartsWith(xbsl::string_view text, xbsl::string_view prefix) {
  return prefix.empty() ||
         (text.size() >= prefix.size() &&
          0 == std::memcmp(text.data(), prefix.data(), prefix.size()));
}

// EndsWith()
//
// Returns whether a given string `text` ends with `suffix`.
inline bool EndsWith(xbsl::string_view text, xbsl::string_view suffix) {
  return suffix.empty() ||
         (text.size() >= suffix.size() &&
          0 == std::memcmp(text.data() + (text.size() - suffix.size()),
                                  suffix.data(), suffix.size())
         );
}

// EqualsIgnoreCase()
//
// Returns whether given ASCII strings `piece1` and `piece2` are equal, ignoring
// case in the comparison.
bool EqualsIgnoreCase(xbsl::string_view piece1, xbsl::string_view piece2);

// StartsWithIgnoreCase()
//
// Returns whether a given ASCII string `text` starts with `prefix`,
// ignoring case in the comparison.
bool StartsWithIgnoreCase(xbsl::string_view text, xbsl::string_view prefix);

// EndsWithIgnoreCase()
//
// Returns whether a given ASCII string `text` ends with `suffix`, ignoring
// case in the comparison.
bool EndsWithIgnoreCase(xbsl::string_view text, xbsl::string_view suffix);

}  // namespace xbsl

#endif  // S2_THIRD_PARTY_XBSL_STRINGS_MATCH_H_
