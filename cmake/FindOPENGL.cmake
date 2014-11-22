if(NOT WITH_OPENGL)
    message(STATUS "No opengl")
    return()
endif()

if(UNIX)
  if(NOT ANDROID)
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    if(NOT GLUT_FOUND)
        message(ERROR " GLUT not found! Install GLUT")
        return()
    endif()
    if(NOTOPENGL_FOUND)
        message(ERROR " OPENGL not found! Install OpenGL")
        return()
    endif()
    set(POPULATION_LIBRARY ${POPULATION_LIBRARY} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
    set(HAVE_OPENGL YES)
    set(HAVE_GLUT YES)
    set(WITH_THREAD YES)
  endif()
endif()

if(WIN32)
  find_package(OpenGL REQUIRED)
  if(NOTOPENGL_FOUND)
        message(ERROR " OPENGL not found! Install OpenGL")
        return()
  endif()
  set(POPULATION_LIBRARY ${POPULATION_LIBRARY} ${OPENGL_LIBRARIES})
  set(HAVE_OPENGL YES)
endif()


#sudo apt-get install libxmu-dev libxi-dev
