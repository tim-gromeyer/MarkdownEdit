#ifndef COMMON_H
#define COMMON_H

#define RECENT_OPENED_LIST_LENGTH 7

#define MD_UNDERLINE true

#if defined(OS_ANDROID) || defined(OS_WASM)
#define SPELLCHECK false
#else
#define SPELLCHECK true
#endif

#if !SPELLCHECK
#define NO_SPELLCHECK
#endif

#endif // COMMON_H
