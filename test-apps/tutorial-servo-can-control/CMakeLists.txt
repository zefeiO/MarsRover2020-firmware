add_library (TutorialServo STATIC)
target_sources (TutorialServo PRIVATE src/TutorialServo.cpp)
target_include_directories (TutorialServo PUBLIC include)
target_link_libraries (TutorialServo PRIVATE mbed-os)

add_executable(tutorial-servo-can-control)
target_sources (tutorial-servo-can-control PRIVATE src/main.cpp)
target_link_libraries(tutorial-servo-can-control
        PRIVATE
        TutorialServo
        CANInterface
        CANMsg
        mbed-os
        uwrt-mars-rover-hw-bridge
        )