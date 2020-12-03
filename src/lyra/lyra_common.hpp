#ifndef LYRA_COMMON_HPP
#define LYRA_COMMON_HPP

// Lyra type def
typedef double        real_t;
typedef unsigned long ul_t;
typedef int           error_t;

// Lyra Macro
#define LYRA_INLINE      inline
#define LYRA_HARD_INLINE LYRA_INLINE __attribute__ ((flatten)) __attribute__ ((always_inline))
#define COLOR_BLACK      "\033[0;30m"
#define COLOR_RED        "\033[0;31m"
#define COLOR_GREEN      "\033[0;32m"
#define COLOR_YELLOW     "\033[0;33m"
#define COLOR_BLUE       "\033[0;34m"
#define COLOR_MAGENTA    "\033[0;35m"
#define COLOR_WHITE      "\033[0;97m"
#define COLOR_DEFAULT    "\033[0;0m"
#define UNDERLINE        "\033[0;4m"
#define BLINK            "\033[0;5m"
#define REVERSE          "\033[1;7m"
#define TREE_BRANCH      "\u251C\u2500\u2500"
#define ENDLINE          COLOR_DEFAULT << std::endl
#define FLUSHLINE        COLOR_DEFAULT << std::flush
#define SEPARATOR        "--------------------------"
#define BEGIN            std::cout << REVERSE << "--> "
#define ENDFUN           std::cout << ENDLINE
#define COUT             std::cout

#define SPC  std::setw (15) <<
#define NEXT " " <<

#define USE_ERROR(X)       \
    if (X == EXIT_FAILURE) \
    {                      \
        return X;          \
    }

#ifdef DEBUG
#define STATUS std::cout << "(" << __FUNCTION__ << ")\t" \
                         << "Status : "
#define INFOS std::cout << "(" << __FUNCTION__ << ")\t" \
                        << " * "
#define ERROR std::cerr << COLOR_RED << "(" << __FUNCTION__ << ")\t" \
                        << " Error : "
#define WARNING std::cout << COLOR_YELLOW << "(" << __FUNCTION__ << ")\t" \
                          << " Warning : "
#else
#define STATUS  std::cout << "Status : "
#define INFOS   std::cout << " * "
#define ERROR   std::cerr << COLOR_RED << "Error : "
#define WARNING std::cout << COLOR_YELLOW << "Warning : "
#endif

#endif  // LYRA_COMMON_HPP