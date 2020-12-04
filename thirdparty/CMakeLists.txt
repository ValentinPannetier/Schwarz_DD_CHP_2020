include(ExternalProject)

option(DOWNLOAD_METIS "Download Metis" OFF)

if (DOWNLOAD_METIS)
    # ###############################################################################
    # # GKLib
    # ###############################################################################

    ExternalProject_Add(
    GKlib

    GIT_REPOSITORY "https://github.com/KarypisLab/GKlib.git"
    GIT_TAG "master"
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    
    #   PREFIX "${CMAKE_BINARY_DIR}/GKLib"        
    DEPENDS ""
    CONFIGURE_COMMAND make config prefix=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/GKlib
    BUILD_COMMAND make
    INSTALL_COMMAND make install


    #   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/GKlib
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/build/GKlib"
    BUILD_IN_SOURCE 1
    )


    # ###############################################################################
    # # METIS
    # ###############################################################################

    ExternalProject_Add(
    Metis

    GIT_REPOSITORY "https://github.com/KarypisLab/METIS.git"
    GIT_TAG "master"
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    
    #   PREFIX "${CMAKE_BINARY_DIR}/Metis"        
    DEPENDS GKlib

    #   BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Metis
    CONFIGURE_COMMAND pwd && make config shared=1 cc=gcc prefix=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Metis gklib_path=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/GKlib
    BUILD_COMMAND     make
    INSTALL_COMMAND   make install


    #   SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Metis"
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/build/Metis"
    BUILD_IN_SOURCE 1
    )
endif(DOWNLOAD_METIS)


# ###############################################################################
# # Scotch
# ###############################################################################

option(DOWNLOAD_SCOTCH "Download Scotch" OFF)

if (DOWNLOAD_SCOTCH)
    ExternalProject_Add(
    Scotch

    GIT_REPOSITORY "https://gitlab.inria.fr/scotch/scotch.git"
    GIT_TAG "master"
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    
    #   PREFIX "${CMAKE_BINARY_DIR}/Scotch"        

    #   BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Scotch
    #   CONFIGURE_COMMAND pwd && make config shared=1 cc=gcc prefix=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Metis gklib_path=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/GKlib
    BUILD_COMMAND cd src && make
    CONFIGURE_COMMAND cd src && cp Make.inc/Makefile.inc.x86-64_pc_linux2 Makefile.inc
    INSTALL_COMMAND cd src && make prefix=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Scotch install


    #   SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Scotch"
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/build/Scotch"
    BUILD_IN_SOURCE 1
    )
endif(DOWNLOAD_SCOTCH)

