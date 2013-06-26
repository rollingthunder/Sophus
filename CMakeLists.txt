SET(PROJECT_NAME Sophus)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)



IF( NOT CMAKE_BUILD_TYPE )
   SET( CMAKE_BUILD_TYPE RelWithDebInfo )
ENDIF()

IF ( NOT MSVC )
   SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
   SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG ")
   SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -DNDEBUG ")

  ADD_DEFINITIONS("-Wall")
ENDIF()


################################################################################
# Create variables used for exporting in SophusConfig.cmake
set( Sophus_LIBRARIES "" )
set( Sophus_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} )

################################################################################




find_package( Eigen REQUIRED )
INCLUDE_DIRECTORIES( ${EIGEN_INCLUDE_DIRS} )

SET( Sophus_INCLUDE_DIRS ${Sophus_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} )

SET (SOURCE_DIR "sophus")
SET (CLASSES  #so2
              se2
              se3
              so3
              scso3
              sim3
)

SET (SOURCES)

FOREACH(class ${CLASSES})
  LIST(APPEND SOURCES ${SOURCE_DIR}/${class}.cpp ${SOURCE_DIR}/${class}.h)
ENDFOREACH(class)

set( Sophus_LIBRARIES ${Sophus_LIBRARIES} ${LIBS} )

INCLUDE_DIRECTORIES(${INCLUDE_DIRS})
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCES})



