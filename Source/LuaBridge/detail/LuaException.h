//------------------------------------------------------------------------------
/*
  https://github.com/vinniefalco/LuaBridge
  
  Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
  Copyright 2008, Nigel Atkinson <suprapilot+LuaCode@gmail.com>

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

#include <exception>
#include <string>

namespace luabridge {
  /// saves stack top in Lua registry to set it cleanly back after pcall.
  /// stores backtrace on error
  class LuaCaller {
  public:
    /// call this before pushing function and args to call
    static void preCall(lua_State* L) {
      // assert(registry[L] == nil) -- check preCall/postCall sequence consistency
      const int top = ::lua_gettop(L);
      ::lua_pushlightuserdata(L, L);
      ::lua_gettable(L, LUA_REGISTRYINDEX);
      assert(lua_isnil(L, -1) == 1);
      lua_pop(L, 1);
       // regitry[L] = stack top -- save stack top to restore
      ::lua_pushlightuserdata(L, L);
      ::lua_pushinteger(L, top);
      ::lua_settable(L, LUA_REGISTRYINDEX);
      // push backtrace function to reference it in error case
      ::lua_pushcfunction(L, LuaBacktrace);
    }

    /// call this after getting the return values from stack
    static void postCall(lua_State* L) {
      // assert(type(registry[L]) == "number") -- check preCall/postCall sequence consistency
      ::lua_pushlightuserdata(L, L);
      ::lua_gettable(L, LUA_REGISTRYINDEX);
      assert(lua_isnumber(L, -1) == 1);
      // top = registry[L]
      const int top = ::lua_tointeger(L, -1);
      // registry[L] = nil
      ::lua_pushlightuserdata(L, L);
      ::lua_pushnil(L);
      ::lua_settable(L, LUA_REGISTRYINDEX);
      // restore stack top
      ::lua_settop(L, top);
    }

  private:
    static int LuaBacktrace(lua_State* L) {
      // top = registry[L]
      ::lua_pushlightuserdata(L, L);
      ::lua_gettable(L, LUA_REGISTRYINDEX);
      assert(lua_isnumber(L, -1) == 1);
      const int top = ::lua_tointeger(L, -1);
      // push errmsg + stack trace
      ::luaL_traceback(L, L, ::lua_tostring(L, -2), top);
      return 1;
    }
  };


class LuaException : public std::exception 
{
private:
  lua_State* m_L;
  std::string m_what;

public:
  //----------------------------------------------------------------------------
  /**
      Construct a LuaException after a lua_pcall().
  */
  LuaException (lua_State* L, int /*code*/)
    : m_L (L)
  {
    whatFromStack ();
  }

  //----------------------------------------------------------------------------

  LuaException (lua_State *L,
                char const*,
                char const*,
                long)
    : m_L (L)
  {
    whatFromStack ();
  }

  //----------------------------------------------------------------------------

  ~LuaException() throw ()
  {
  }

  //----------------------------------------------------------------------------

  char const* what() const throw ()
  {
    return m_what.c_str();
  }

  //============================================================================
  /**
      Throw an exception.

      This centralizes all the exceptions thrown, so that we can set
      breakpoints before the stack is unwound, or otherwise customize the
      behavior.
  */
  template <class Exception>
  static void Throw (Exception e)
  {
    throw e;
  }

  //----------------------------------------------------------------------------
  /**
      Wrapper for lua_pcall that throws.
  */
  static void pcall (lua_State* L, int nargs = 0, int nresults = 0)
  {
    int code = lua_pcall (L, nargs, nresults, -(nargs + 2)); // error handler is pushed before function and args

    if (code != LUABRIDGE_LUA_OK)
      Throw (LuaException (L, code));
  }

  //----------------------------------------------------------------------------
  /**
      Initializes error handling. Subsequent Lua errors are translated to C++ exceptions.
  */
  static void enableExceptions (lua_State* L)
  {
    lua_atpanic (L, throwAtPanic);
  }

protected:
  void whatFromStack ()
  {
    if (lua_gettop (m_L) > 0)
    {
      char const* s = lua_tostring (m_L, -1);
      m_what = s ? s : "";
    }
    else
    {
      // stack is empty
      m_what = "missing error";
    }
    LuaCaller::postCall(m_L);
  }

private:
  static int throwAtPanic (lua_State* L)
  {
    throw LuaException (L, -1);
  }
};

//----------------------------------------------------------------------------
/**
    Initializes error handling. Subsequent Lua errors are translated to C++ exceptions.
*/
static void enableExceptions (lua_State* L)
{
  LuaException::enableExceptions (L);
}

} // namespace luabridge
