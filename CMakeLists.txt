cmake_minimum_required(VERSION 3.24)
set(CMAKE_INSTALL_PREFIX "/Users/eprager/local")

file(GLOB HEADERS
	 RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	 CONFIGURE_DEPENDS
	 "include/*.h"
)

file(GLOB SOURCES
	 RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	 CONFIGURE_DEPENDS
	 "src/*.cpp"
	 "imgui/*.cpp"
)

project(evk_imgui CXX)

add_library(${PROJECT_NAME}
            "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCES}"
            "${CMAKE_CURRENT_SOURCE_DIR}/${HEADERS}"
            )

target_include_directories(${PROJECT_NAME} PUBLIC
                           "${CMAKE_CURRENT_SOURCE_DIR}/include/"
                           "/usr/local/include/"
                           "/Users/eprager/local/include/"
                           "/opt/local/include/"
                           "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
                           )

target_link_directories(${PROJECT_NAME} PUBLIC
                        "/usr/local/lib/"
                        "/Users/eprager/local/lib/"
                        "/opt/local/lib/"
                        )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME}
                      vulkan
                      mattresses
                      evk
                      )

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION "lib"
        PUBLIC_HEADER DESTINATION "include/evk_imgui"
)
