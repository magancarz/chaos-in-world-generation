target_sources(chaos-in-world-generation-lib PRIVATE
        imgui.cpp
        imgui_draw.cpp
        imgui_impl_glfw.cpp
        imgui_impl_opengl3.cpp
        imgui_stdlib.cpp
        imgui_tables.cpp
        imgui_widgets.cpp)

target_include_directories(chaos-in-world-generation-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})