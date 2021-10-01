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
// File: macros.h
// -----------------------------------------------------------------------------
//
// This header file defines the set of language macros used within Abseil code.
// For the set of macros used to determine supported compilers and platforms,
// see xbsl/base/config.h instead.
//
// This code is compiled directly on many platforms, including client
// platforms like Windows, Mac, and embedded systems.  Before making
// any changes here, make sure that you're not breaking any platforms.

#ifndef S2_THIRD_PARTY_XBSL_BASE_MACROS_H_
#define S2_THIRD_PARTY_XBSL_BASE_MACROS_H_

#include <cassert>
#include <cstddef>

#include "s2/third_party/xbsl/base/port.h"

// XBSL_ARRAYSIZE()
//
// Returns the number of elements in an array as a compile-time constant, which
// can be used in defining new arrays. If you use this macro on a pointer by
// mistake, you will get a compile-time error.
#define XBSL_ARRAYSIZE(array) \
  (sizeof(::xbsl::macros_internal::ArraySizeHelper(array)))

namespace xbsl {
namespace macros_internal {
// Note: this internal template function declaration is used by XBSL_ARRAYSIZE.
// The function doesn't need a definition, as we only use its type.
template <typename T, size_t N>
auto ArraySizeHelper(const T (&array)[N]) -> char (&)[N];
}  // namespace macros_internal
}  // namespace xbsl

// TODO(b/62370839): Replace arraysize() with XBSL_ARRAYSIZE().
template <typename T, size_t N>
auto ArraySizeHelper(const T (&array)[N]) -> char (&)[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

// kLinkerInitialized
//
// An enum used only as a constructor argument to indicate that a variable has
// static storage duration, and that the constructor should do nothing to its
// state. Use of this macro indicates to the reader that it is legal to
// declare a static instance of the class, provided the constructor is given
// the xbsl::base_internal::kLinkerInitialized argument.
//
// Normally, it is unsafe to declare a static variable that has a constructor or
// a destructor because invocation order is undefined. However, if the type can
// be zero-initialized (which the loader does for static variables) into a valid
// state and the type's destructor does not affect storage, then a constructor
// for static initialization can be declared.
//
// Example:
//       // Declaration
//       explicit MyClass(xbsl::base_internal:LinkerInitialized x) {}
//
//       // Invocation
//       static MyClass my_global(xbsl::base_internal::kLinkerInitialized);
namespace xbsl {
namespace base_internal {
enum LinkerInitialized {
  kLinkerInitialized = 0,
  LINKER_INITIALIZED = 0,
};
}  // namespace base_internal
}  // namespace xbsl

namespace base {
using xbsl::base_internal::LinkerInitialized;
using xbsl::base_internal::LINKER_INITIALIZED;
}  // namespace base

// XBSL_FALLTHROUGH_INTENDED
//
// Annotates implicit fall-through between switch labels, allowing a case to
// indicate intentional fallthrough and turn off warnings about any lack of a
// `break` statement. The XBSL_FALLTHROUGH_INTENDED macro should be followed by
// a semicolon and can be used in most places where `break` can, provided that
// no statements exist between it and the next switch label.
//
// Example:
//
//  switch (x) {
//    case 40:
//    case 41:
//      if (truth_is_out_there) {
//        ++x;
//        XBSL_FALLTHROUGH_INTENDED;  // Use instead of/along with annotations
//                                    // in comments
//      } else {
//        return x;
//      }
//    case 42:
//      ...
//
// Notes: when compiled with clang in C++11 mode, the XBSL_FALLTHROUGH_INTENDED
// macro is expanded to the [[clang::fallthrough]] attribute, which is analysed
// when  performing switch labels fall-through diagnostic
// (`-Wimplicit-fallthrough`). See clang documentation on language extensions
// for details:
// http://clang.llvm.org/docs/AttributeReference.html#fallthrough-clang-fallthrough
//
// When used with unsupported compilers, the XBSL_FALLTHROUGH_INTENDED macro
// has no effect on diagnostics. In any case this macro has no effect on runtime
// behavior and performance of code.
// TODO(b/62370839): Replace FALLTHROUGH_INTENDED with
// XBSL_FALLTHROUGH_INTENDED.
#if defined(__clang__) && defined(__has_warning)
#if __has_feature(cxx_attributes) && __has_warning("-Wimplicit-fallthrough")
#define FALLTHROUGH_INTENDED [[clang::fallthrough]]
#endif
#elif defined(__GNUC__) && __GNUC__ >= 7
#define FALLTHROUGH_INTENDED [[gnu::fallthrough]]
#endif

#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED do { } while (0)
#endif
#ifdef XBSL_FALLTHROUGH_INTENDED
#error "XBSL_FALLTHROUGH_INTENDED should not be defined."
#endif

// TODO(user): Use c++17 standard [[fallthrough]] macro, when supported.
#if defined(__clang__) && defined(__has_warning)
#if __has_feature(cxx_attributes) && __has_warning("-Wimplicit-fallthrough")
#define XBSL_FALLTHROUGH_INTENDED [[clang::fallthrough]]
#endif
#elif defined(__GNUC__) && __GNUC__ >= 7
#define XBSL_FALLTHROUGH_INTENDED [[gnu::fallthrough]]
#endif

#ifndef XBSL_FALLTHROUGH_INTENDED
#define XBSL_FALLTHROUGH_INTENDED \
  do {                            \
  } while (0)
#endif

// XBSL_DEPRECATED()
//
// Marks a deprecated class, struct, enum, function, method and variable
// declarations. The macro argument is used as a custom diagnostic message (e.g.
// suggestion of a better alternative).
//
// Example:
//
//   class XBSL_DEPRECATED("Use Bar instead") Foo {...};
//   XBSL_DEPRECATED("Use Baz instead") void Bar() {...}
//
// Every usage of a deprecated entity will trigger a warning when compiled with
// clang's `-Wdeprecated-declarations` option. This option is turned off by
// default, but the warnings will be reported by clang-tidy.
#if defined(__clang__) && __cplusplus >= 201103L
#define XBSL_DEPRECATED(message) __attribute__((deprecated(message)))
#endif

#ifndef XBSL_DEPRECATED
#define XBSL_DEPRECATED(message)
#endif

// XBSL_BAD_CALL_IF()
//
// Used on a function overload to trap bad calls: any call that matches the
// overload will cause a compile-time error. This macro uses a clang-specific
// "enable_if" attribute, as described at
// http://clang.llvm.org/docs/AttributeReference.html#enable-if
//
// Overloads which use this macro should be bracketed by
// `#ifdef XBSL_BAD_CALL_IF`.
//
// Example:
//
//   int isdigit(int c);
//   #ifdef XBSL_BAD_CALL_IF
//   int isdigit(int c)
//     XBSL_BAD_CALL_IF(c <= -1 || c > 255,
//                       "'c' must have the value of an unsigned char or EOF");
//   #endif // XBSL_BAD_CALL_IF

#if defined(__clang__)
# if __has_attribute(enable_if)
#  define XBSL_BAD_CALL_IF(expr, msg) \
    __attribute__((enable_if(expr, "Bad call trap"), unavailable(msg)))
# endif
#endif

// XBSL_ASSERT()
//
// In C++11, `assert` can't be used portably within constexpr functions.
// XBSL_ASSERT functions as a runtime assert but works in C++11 constexpr
// functions.  Example:
//
// constexpr double Divide(double a, double b) {
//   return XBSL_ASSERT(b != 0), a / b;
// }
//
// This macro is inspired by
// https://akrzemi1.wordpress.com/2017/05/18/asserts-in-constexpr-functions/
#if defined(NDEBUG)
#define XBSL_ASSERT(expr) (false ? (void)(expr) : (void)0)
#else
#define XBSL_ASSERT(expr)              \
  (XBSL_PREDICT_TRUE((expr)) ? (void)0 \
                             : [] { assert(false && #expr); }())  // NOLINT
#endif

#ifdef XBSL_HAVE_EXCEPTIONS
#define XBSL_INTERNAL_TRY try
#define XBSL_INTERNAL_CATCH_ANY catch (...)
#define XBSL_INTERNAL_RETHROW do { throw; } while (false)
#else  // XBSL_HAVE_EXCEPTIONS
#define XBSL_INTERNAL_TRY if (true)
#define XBSL_INTERNAL_CATCH_ANY else if (false)
#define XBSL_INTERNAL_RETHROW do {} while (false)
#endif  // XBSL_HAVE_EXCEPTIONS

#endif  // S2_THIRD_PARTY_XBSL_BASE_MACROS_H_
