add_library(WorkflowQt STATIC)
add_library(Rapid::WorkflowQt ALIAS WorkflowQt)

target_sources(WorkflowQt
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ISessionAnalyzeWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionAnalyzeWorkflow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionAnalyzeWorkflow.hpp
)

target_link_libraries(WorkflowQt
PUBLIC
    Rapid::CommonQt
    Rapid::Workflow
    Qt6::Gui
    Qt6::GuiPrivate
    Qt6::Widgets
)

target_include_directories(WorkflowQt
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)

target_compile_definitions(WorkflowQt PRIVATE QT_NO_KEYWORDS)
