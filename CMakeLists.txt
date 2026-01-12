cmake_minimum_required(VERSION 3.10)  # Минимальная версия CMake

project(Oppo)  # Название вашего проекта (можно изменить)

# Создайте исполняемый файл из Program.cpp
add_executable(Program Program.cpp)

# Если файлы данных нужны в build-директории, скопируйте их
configure_file(date1.txt date1.txt COPYONLY)
configure_file(date2.txt date2.txt COPYONLY)
configure_file(date3.txt date3.txt COPYONLY)