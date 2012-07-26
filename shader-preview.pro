TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .
LIBS += -lGLU

QT += opengl

QMAKE_CXXFLAGS += -g2 -std=c++11 -Wextra -DX86_ASSEMBLY -DX64_ASSEMBLY -Wshadow

# Input
HEADERS += main_window.hpp renderer.hpp stage_tab.hpp types.hpp popup_tree.hpp shader_edit.hpp uniform.hpp color_buffer.hpp vertex_info.hpp opengl.hpp dialogs.hpp texture_management.hpp trackable.hpp
SOURCES += main.cpp main_window.cpp renderer.cpp stage_tab.cpp types.cpp popup_tree.cpp shader_edit.cpp uniform.cpp color_buffer.cpp vertex_info.cpp dialogs.cpp texture_management.cpp trackable.cpp
