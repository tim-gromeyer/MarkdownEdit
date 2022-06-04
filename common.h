#ifndef COMMON_H
#define COMMON_H

#define RECENT_OPENED_LIST_LENGTH 7

#define MD_UNDERLINE true

#define SPELLCHECK true  // Not avaiable on android and web assembly

#if !SPELLCHECK
#define NO_SPELLCHECK
#endif

#endif // COMMON_H
