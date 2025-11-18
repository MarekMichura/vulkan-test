#ifndef LIB_UTILS_DEBUG
#define LIB_UTILS_DEBUG

constexpr bool Debug =
#ifdef DEBUG
    true;
#else
    false;
#endif

constexpr bool PrintTable =
#if PRINT_TABLE
    true;
#else
    false;
#endif

#endif /* LIB_UTILS_DEBUG */
