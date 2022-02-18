add_executable(cosmic_poll
    main.cpp
    program_stoppable_sleep.cpp
    observation_logger.cpp
    log_db_engine.cpp
    create_log_db.cpp
    memory_monitor.cpp
    command_line.cpp
    compile_paths.cpp
)

target_link_libraries(cosmic_poll
    PRIVATE
        fmt::fmt
        sqlite_wrapper
        CLI11::CLI11
)

target_compile_features(cosmic_poll PRIVATE cxx_std_20)
