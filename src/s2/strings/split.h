// Copyright 2017 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS-IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef S2_STRINGS_SPLIT_H_
#define S2_STRINGS_SPLIT_H_

#include <functional>
#include <string>
#include <vector>

namespace strings {

std::vector<std::string> Split(
    std::string const& text, char delim,
    std::function<bool(std::string const&)> predicate);
std::vector<std::string> Split(std::string const& text, char delim);

struct SkipEmpty {
  bool operator()(std::string const& s) { return !s.empty(); }
};

}  // namespace strings

#endif  // S2_STRINGS_SPLIT_H_