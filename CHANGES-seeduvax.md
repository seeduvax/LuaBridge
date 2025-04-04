# LuaBridge seeduvax fork specifics change log

## tag LuaBridge_seeduvax-2.6.5

changed stack consistency check in a more explicit thread id check

## tag LuaBridge_seeduvax-2.6.4

attach backtrace to Lua error message in LuaException + check call stack consistency of Lua calling through LuaBridge

## tag LuaBridge_seeduvax-2.6.3

added specific case for Lua models

## tag LuaBridge_seeduvax-2.6.2

### seeduvax#4: missing stack pop on exception handling 

use a more or less safe and consistent way to have class keys work across lib
boundaries. requires c++14. Pull request from flohgh.

## tag LuaBridge_seeduvax-2.6.1

### seeduvax#3: missing stack pop on exception handling 

Many exceptions lead to a lua stack overflow.
Disccovery and fix by flohgh

## tag LuaBridge_seeduvax-2.6.0

### seeduvax#2: enable additional index/newindex methods for a C++ class tree

Introduced to let C++ object implementing a named composite pattern to bind its
component hierarchy in the lua space, letting the user walk the component
hierarchy using the dot operator like in any lua table.
May be also a way to solve issue [#197][I197]

### seeduvax#1: compilation switch to disable stack specialization for int

Some users need a specific alternate stack specialization to deal with int64
types and manage compatiblité with both luatjit and pre lua 5.1 and post lua 5.1

## new branch seeduvax-2.6

* forked from vinniefalco's 2.6

[I197]:  https://github.com/vinniefalco/LuaBridge/issues/197
