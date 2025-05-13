cmake_minimum_required(VERSION 3.15)
project(GEMS3 LANGUAGES CXX)

# Настройка стандарта C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Поиск SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Исходные файлы
set(SOURCES
    Board.cpp
    Game.cpp
    Gem.cpp
    main.cpp
)

# Заголовочные файлы
set(HEADERS
    Board.h
    Config.h
    Game.h
    Gem.h
)

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Подключение SFML
target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

# Настройки компилятора для MSVC
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W3           # Уровень предупреждений 3
        /permissive-  # Строгое соответствие стандартам
        /sdl          # Проверки безопасности
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -pedantic
    )
endif()

# Настройки конфигураций
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# Оптимизации для Release
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:
        /O2
        /GL
    >
)

# Настройки линковщика
target_link_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:
        /LTCG
        /OPT:REF
        /OPT:ICF
    >
)

# Рабочая директория для доступа к ресурсам (например, gems.png)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Для копирования файла gems.png в выходную директорию (опционально)
configure_file(
    "${CMAKE_SOURCE_DIR}/gems.png"
    "${CMAKE_BINARY_DIR}/gems.png"
    COPYONLY
)