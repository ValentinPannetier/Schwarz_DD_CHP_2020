#ifndef LYRA_COMMON_HPP
#define LYRA_COMMON_HPP

// Lyra type def
typedef double          real_t;
typedef unsigned long   ul_t;

// Lyra Macro
#define LYRA_HARD_INLINE    inline __attribute__((flatten)) __attribute__((always_inline))
#define LYRA_INLINE         inline


#define COLOR_BLACK         "\033[0;30m"
#define COLOR_RED           "\033[0;31m"
#define COLOR_GREEN         "\033[0;32m"
#define COLOR_YELLOW        "\033[0;33m"
#define COLOR_BLUE          "\033[0;34m"
#define COLOR_MAGENTA       "\033[0;35m"
#define COLOR_WHITE         "\033[0;97m"
#define COLOR_DEFAULT       "\033[0;0m"

#define UNDERLINE           "\033[0;4m"
#define BLINK               "\033[0;5m"
#define REVERSE             "\033[1;7m"

#define TREE_BRANCH         "\u251C\u2500\u2500"

#define ENDLINE             COLOR_DEFAULT << std::endl
#define FLUSHLINE           COLOR_DEFAULT << std::flush
#define SEPARATOR           "--------------------------"
#define BEGIN               std::cout << REVERSE << "--> "
#define ENDFUN              std::cout << ENDLINE
#define COUT                std::cout

#ifdef DEBUG
#define STATUS              std::cout                   << "(" << __FUNCTION__ << ")\t" << " STATUS : "
#define INFOS               std::cout                   << "(" << __FUNCTION__ << ")\t" << "* "
#define ERROR               std::cerr << COLOR_RED      << "(" << __FUNCTION__ << ")\t" << " ERROR : "
#define WARNING             std::cout << COLOR_YELLOW   << "(" << __FUNCTION__ << ")\t" << " WARNING : "
#else
#define STATUS              std::cout                   << "STATUS : "
#define INFOS               std::cout                   << " * "
#define ERROR               std::cerr << COLOR_RED      << "ERROR : "
#define WARNING             std::cout << COLOR_YELLOW   << "WARNING : "
#endif

#endif // LYRA_COMMON_HPP