cmake_minimum_required(VERSION 3.1)
project(mec_http_server)

set(CMAKE_CXX_STANDARD 11)

include_directories("yourcaffe/include") 
link_directories("yourcaffe/lib")   # libcaffe.so

# If cuda is used
# include_directories("/usr/local/cuda/include")
# link_directories("/usr/local/cuda/lib64") 

file(GLOB SRC_mec_http_server
    "src/mec_http_server/*.hpp"
    "src/mec_http_server/*.cpp"
)

set(SOURCE_FILES ${SRC_mec_http_server})
add_executable(mec_http_server ${SOURCE_FILES})

# OpenCV
find_package(OpenCV REQUIRED)
target_link_libraries(mec_http_server ${OpenCV_LIBS} )

# Other libraries, cublas is for cuda
target_link_libraries(mec_http_server caffe cublas boost_system glog)
