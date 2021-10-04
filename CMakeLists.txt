project(gocqhttp)

include_directories("src")
set(SOURCE
    "src/gocqhttp_API.c"
    "src/gocqhttp_err.c"
    "src/gocqhttp_Event.c"
    "src/URLcode.c"
)

add_library(gocqhttplib STATIC ${SOURCE})
target_link_directories(gocqhttplib PUBLIC "src/")
add_executable(gocqhttp "src/main.c")
target_link_libraries(gocqhttp PRIVATE gocqhttplib)
