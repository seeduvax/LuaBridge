//------------------------------------------------------------------------------
/*
  https://github.com/vinniefalco/LuaBridge
  
  Copyright 2020, Dmitry Tarakanov
  Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>

  License: The MIT License (http://www.opensource.org/licenses/mit-license.php)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
//==============================================================================

#pragma once

#include <typeinfo>

namespace luabridge {
namespace detail {

/**
 * A unique key for a type name in a metatable.
 */
inline const void* getTypeKey ()
{
  return reinterpret_cast <void*> (0x71);
}

/**
 * The key of a const table in another metatable.
 */
inline const void* getConstKey ()
{
  return reinterpret_cast <void*> (0xc07);
}

/**
 * The key of a class table in another metatable.
 */
inline const void* getClassKey ()
{
  return reinterpret_cast <void*> (0xc1a);
}

/**
 * The key of a propget table in another metatable.
 */
inline const void* getPropgetKey ()
{
  return reinterpret_cast <void*> (0x6e7);
}

/**
 * The key of a propset table in another metatable.
 */
inline const void* getPropsetKey ()
{
  return reinterpret_cast <void*> (0x5e7);
}

/**
 * The key of a static table in another metatable.
 */
inline const void* getStaticKey ()
{
  return reinterpret_cast <void*> (0x57a);
}

/**
 * The key of a parent table in another metatable.
 */
inline const void* getParentKey ()
{
  return reinterpret_cast <void*> (0xdad);
}

/**
 * The key of the index fall back in another metatable.
 */
inline const void* getIndexKey ()
{
  return reinterpret_cast <void*> (0x81ca);
}
/**
 * The key of the new index fall back in another metatable.
 */
inline const void* getNewIndexKey ()
{
  return reinterpret_cast <void*> (0x8107);
}

#ifndef CPP_FOR_LIBCLANG

static constexpr uint32_t nextSeed(uint32_t seed, const char* currentS) {
    return *currentS != '\0' ? nextSeed(static_cast<uint32_t>(static_cast<uint32_t>(seed ^ static_cast<uint8_t>(*currentS)) * 16777619u), currentS + 1) : seed;
}

// use compile-time hash function. fnv1a has good enough distribution. don't use std::type_info.hash_code() because there 
// can be inconsistencies across compilers/libs used and we want this to be robust across shared libs boundaries
// (for example std::type_info<T>.hash_code() yields different results in libc++ and libstdc++
static constexpr uint32_t fnv1a(const char* s) noexcept
{
    return nextSeed(2166136261u, s);
}

template <class T>
static constexpr size_t typeHash() noexcept
{
    // no real other way to get type name in a constexpr context without doing this. at least on modern clang and g++ this return the same result, so it should be safe
    return fnv1a(typeid(T).name());
}

/**
    Get the key for the static table in the Lua registry.
    The static table holds the static data members, static properties, and
    static member functions for a class.
*/
template <class T>
void const* getStaticRegistryKey ()
{
    static auto value = typeHash<T>();
    return reinterpret_cast<void*>(value);
}

/** Get the key for the class table in the Lua registry.
    The class table holds the data members, properties, and member functions
    of a class. Read-only data and properties, and const member functions are
    also placed here (to save a lookup in the const table).
*/
template<class T>
void const* getClassRegistryKey ()
{
    static auto value = typeHash<T>() ^ 1;
    return reinterpret_cast<void*>(value);
}

/** Get the key for the const table in the Lua registry.
    The const table holds read-only data members and properties, and const
    member functions of a class.
*/
template<class T>
void const* getConstRegistryKey ()
{
    static auto value = typeHash<T>() ^ 2;
    return reinterpret_cast<void*>(value);
}

#else
// this code will never actually run, it's there to ease ASTRE Lua model parsing on all platforms (some are not fully compliant with clang std=c++14)
template <class T>
void const* getStaticRegistryKey ()
{
    return reinterpret_cast <void*> (0xcaca);
}

template<class T>
void const* getClassRegistryKey ()
{
    return reinterpret_cast <void*> (0xcaca);
}

template<class T>
void const* getConstRegistryKey ()
{
    return reinterpret_cast <void*> (0xcaca);
}
#endif // CPP_FOR_LIBCLANG

} // namespace detail

} // namespace luabridge
