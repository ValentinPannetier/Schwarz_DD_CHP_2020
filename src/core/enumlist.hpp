#ifndef SRC_LYRA_CORE_ENUMLIST_HPP
#define SRC_LYRA_CORE_ENUMLIST_HPP

/**
 * @brief The PTAG enum : tag points
 */
enum PTAG
{
    PT_NONE      = 0x0,   //! No tag
    PT_SHARED    = 0x1,   //! Shared points
    PT_PHYSICAL  = 0x2,   //!
    PT_VIRTUAL   = 0x4,   //!
    PT_SEND      = 0x8,   //!
    PT_RECEIVE   = 0x10,  //!
    PT_DIRICHLET = 0x20,  //!
    PT_NEUMANN   = 0x40,  //!

    // Several usefull definitions
    PT_ROBIN   = PT_DIRICHLET | PT_NEUMANN,
    PT_FIRST   = PT_NONE,
    PT_LAST    = PT_NEUMANN,
    PT_DEFAULT = PT_NONE,
};

/**
 * @brief The DIR enum : what you think !
 */
enum DIR
{
    D_LEFT    = 0x0,
    D_RIGHT   = 0x1,
    D_UP      = 0x2,
    D_BOTTOM  = 0x3,
    D_NOT_SET = 0x4,
    D_DEFAULT = D_NOT_SET
};

#endif /* SRC_LYRA_CORE_ENUMLIST_HPP */
