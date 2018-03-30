cmake_minimum_required (VERSION 2.6)
project (pigeonwar)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

#find_package(SFML 2 REQUIRED network)
##include_directories(${SFML_INCLUDE_DIR})

add_subdirectory(../Libraries/oxygine-framework/ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

link_directories(/usr/lib/x86_64-linux-gnu/)
include_directories(/home/abrdej/Libraries/lib/)
include_directories(/home/abrdej/Libraries/asio/asio/include/)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OXYGINE_CXX_FLAGS}")

add_executable(pigeonwar src/main.cpp
        src/core.cpp
		src/res.cpp
		src/gui/board_panel.cpp
		src/gui/buttons_panel.cpp
		src/sprites/entity_holder.cpp
		src/sprites/drawer_factory.cpp
		src/sprites/sprites_manager.cpp
		src/client.cpp
        src/animations/animations_handlers.cpp
        src/animations/animations_service.cpp
        src/messages/message_makers.cpp
        src/utils/message_joiner.cpp
		src/utils/message_joiner.h 
		src/utils/threadsafe_queue.h
		src/game.cpp
		src/game.h
		src/messages/json_message_conversions.h
		src/board_container.h
		src/game_state.h
		src/states.h
		src/utils/utils.h
		src/gui/view_constants.h

		)


target_link_libraries(pigeonwar ${OXYGINE_CORE_LIBS})
#target_link_libraries(pigeonwar ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} ${PLATFORM_LIBS} ${Boost_LOG_LIBRARY})
target_link_libraries(pigeonwar sfml-network sfml-system)
target_link_libraries(pigeonwar enet)

if (WIN32) #disable console mode for VC++
	set_target_properties(pigeonwar PROPERTIES WIN32_EXECUTABLE TRUE)
endif(WIN32)



if (EMSCRIPTEN)
	SET(CMAKE_EXECUTABLE_SUFFIX ".html")	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --js-opts 0 -O2 -g0 --memory-init-file 1 -s TOTAL_MEMORY=1073741824 -s ALIASING_FUNCTION_POINTERS=0 -s DEMANGLE_SUPPORT=1 -s DISABLE_EXCEPTION_CATCHING=0 -s NO_EXIT_RUNTIME=1 -s FORCE_FILESYSTEM=1 -s DEMANGLE_SUPPORT=1  -s WARN_ON_UNDEFINED_SYMBOLS=1 -I/usr/lib/x86_64-linux-gnu/ -I/home/abrdej/Libraries/lib/ -s USE_PTHREADS=1 --memory-init-file 0 -s TOTAL_MEMORY=50331648")
	em_link_pre_js(pigeonwar  ${OXYGINE_JS_LIBRARIES}  ${CMAKE_CURRENT_SOURCE_DIR}/data.js)
endif(EMSCRIPTEN)
