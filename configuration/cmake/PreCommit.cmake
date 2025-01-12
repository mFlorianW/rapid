set(_THIS_MODULE_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")

macro(setup_precommit)
    message(STATUS "Install pre-commit")
    execute_process(
        COMMAND "${_THIS_MODULE_BASE_DIR}/precommit/PreCommitInstall.sh"
        WORKING_DIRECTORY "${_THIS_MODULE_BASE_DIR}/precommit/"
        ERROR_VARIABLE PRE_COMMIT_INSTALL_ERROR
    )

    if(PRE_COMMIT_INSTALL_ERROR)
        message(FATAL_ERROR "Install of pre-commit failed. Error: ${PRE_COMMIT_INSTALL_ERROR}")
    endif()
endmacro()
