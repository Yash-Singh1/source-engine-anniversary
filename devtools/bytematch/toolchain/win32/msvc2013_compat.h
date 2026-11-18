/* C++11 spellings this tree uses that MSVC 2013 lacks. Force-included (/FI) by
   --retail-win32 only; defines macros, emits no code. */
#pragma once
#if defined( _MSC_VER ) && _MSC_VER < 1900
#define _ALLOW_KEYWORD_MACROS
#define alignas( x ) __declspec( align( x ) )
#define alignof __alignof
#endif
