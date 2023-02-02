project(gocqhttp)

set(SOURCE
    "gocqhttp_API.c"
    "gocqhttp_Event.c"
    "URLcode.c"
    "AnaJSON.c"
    "Log.c"
    "robot.c"
    "Tool.c"
    "win.c"
    "function.c"
)

add_library(gocqhttplib STATIC ${SOURCE})
add_executable(gocqhttp "main.c")
target_link_libraries(gocqhttp PRIVATE gocqhttplib)
