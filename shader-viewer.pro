TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .
LIBS += -lGLU

QT += opengl

QMAKE_CXXFLAGS += -g2 -std=c++11 -Wextra

# Input
HEADERS += gl_output.hpp application.hpp textures.hpp popup_tree.hpp uniforms.hpp dialogs.hpp
SOURCES += main.cpp textures.cpp gl_output.cpp application.cpp popup_tree.cpp uniforms.cpp dialogs.cpp
