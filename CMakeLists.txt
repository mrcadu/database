cmake_minimum_required(VERSION 3.19)
project(database C)

set(CMAKE_C_STANDARD 11)

add_executable(database main.c SqlCompiler.c InputBuffer.c InputBuffer.h SqlCompiler.h PrepareResult.c PrepareResult.h Statement.c Statement.h SqlParser.c SqlParser.h Row.c Row.h Table.c Table.h ExecuteResult.c ExecuteResult.h Pager.c Pager.h Cursor.c Cursor.h)