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

#include "s2/third_party/xbsl/strings/match.h"

#include "s2/third_party/xbsl/strings/internal/memutil.h"

namespace xbsl {

bool EqualsIgnoreCase(xbsl::string_view piece1, xbsl::string_view piece2) {
  return (piece1.size() == piece2.size() &&
          0 == xbsl::strings_internal::memcasecmp(piece1.data(), piece2.data(),
                                                  piece1.size()));
  // memcasecmp uses xbsl::ascii_tolower().
}

bool StartsWithIgnoreCase(xbsl::string_view text, xbsl::string_view prefix) {
  return (text.size() >= prefix.size()) &&
         EqualsIgnoreCase(text.substr(0, prefix.size()), prefix);
}

bool EndsWithIgnoreCase(xbsl::string_view text, xbsl::string_view suffix) {
  return (text.size() >= suffix.size()) &&
         EqualsIgnoreCase(text.substr(text.size() - suffix.size()), suffix);
}

}  // namespace xbsl