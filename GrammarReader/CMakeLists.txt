cmake_minimum_required(VERSION 3.10)

#имя проекта
project(GrammarReader)

#стандарт с++
set(CMAKE_CXX_STANDARD 11)

#добавление исходных файлов
set(SOURCE_FILES
	
	src/GrammarReader.cpp
	src/GrammarReader.h
)

#добавление исполняемого файла
add_executable(GrammarReader ${SOURCE_FILES})