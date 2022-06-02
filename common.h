#ifndef COMMON_H
#define COMMON_H

#define RECENT_OPENED_LIST_LENGTH 7

#ifdef OS_ANDROID
#define SPELLCHECK false
#else
#define SPELLCHECK true
#endif

#if !SPELLCHECK
#define NO_SPELLCHECK
#endif

#endif // COMMON_H
