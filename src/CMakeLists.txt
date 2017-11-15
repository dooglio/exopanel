project( exopanel )

include_directories(
    ${PROJECT_BINARY_DIR}
)

configure_file(
		version.h.in
		${PROJECT_BINARY_DIR}/version.h
		)

set( HEADER_FILES
    ${PROJECT_BINARY_DIR}/version.h
    MainWindow.h
)

add_executable( ${PROJECT_NAME} WIN32
    ${HEADER_FILES}
    MainWindow.cpp
    MainWindow.ui
    main.cpp
    resources.qrc
)

target_link_libraries( ${PROJECT_NAME}
    ${QT_LIBRARIES}
)

install(
  TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin
)

install(
    FILES images/exodus.png DESTINATION share/pixmaps/
    RENAME exopanel.png
)

