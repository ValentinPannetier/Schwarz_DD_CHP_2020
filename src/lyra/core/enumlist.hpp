#ifndef SRC_LYRA_CORE_ENUMLIST_HPP
#define SRC_LYRA_CORE_ENUMLIST_HPP

typedef enum
{
    PT_NONE         = 0x0,
    PT_DIRICHLET    = 0x1,
    PT_NEUMANN      = 0x2,
    PT_ROBIN        = PT_DIRICHLET | PT_NEUMANN,
    PT_PHYSICAL     = 0x4,
    PT_P_DIRICHLET  = PT_DIRICHLET | PT_PHYSICAL,  // 5     BORD PHYSIQUE   + CONDITION DIRICHLET
    PT_P_NEUMANN    = PT_NEUMANN | PT_PHYSICAL,    // 6     BORD PHYSIQUE   + CONDITION NEUMANN
    PT_P_ROBIN      = PT_ROBIN | PT_PHYSICAL,      // 7     BORD PHYSIQUE   + CONDITION ROBIN
    PT_VIRTUAL      = 0x8,
    PT_V_DIRICHLET  = PT_DIRICHLET | PT_VIRTUAL,  // 9     BORD VIRTUEL    + CONDITION DIRICHLET
    PT_V_NEUMANN    = PT_NEUMANN | PT_VIRTUAL,    // 10    BORD VIRTUEL    + CONDITION NEUMANN
    PT_V_ROBIN      = PT_ROBIN | PT_VIRTUAL,      // 11    BORD VIRTUEL    + CONDITION ROBIN
    PT_INTERPOLATE  = 0xc,
    PT_IV_DIRICHLET = PT_V_DIRICHLET | PT_INTERPOLATE,
    PT_IV_NEUMANN   = PT_V_NEUMANN | PT_INTERPOLATE,
    PT_IV_ROBIN     = PT_V_ROBIN | PT_INTERPOLATE,

    PT_FIRST   = PT_NONE,
    PT_LAST    = PT_IV_ROBIN,
    PT_DEFAULT = PT_NONE
} PTAG;

typedef enum
{
    D_LEFT   = 0x0,
    D_RIGHT  = 0x1,
    D_UP     = 0x2,
    D_BOTTOM = 0x3
} DIR;

typedef enum
{
    C_EMPTY  = 0x0,
    C_LINE   = 0x1,
    C_SQUARE = 0x2,
    C_CUBE   = 0x3,

    C_FIRST   = C_EMPTY,
    C_LAST    = C_CUBE,
    C_DEFAULT = C_EMPTY
} CTYPE;

typedef enum
{
    SOLVER_NONE,
    SOLVER_CG,
    SOLVER_BICGSTAB
} STYPE;

#endif /* SRC_LYRA_CORE_ENUMLIST_HPP */