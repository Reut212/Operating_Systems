project(ex7)

cmake_minimum_required(VERSION 3.13)

cmake_policy(SET CMP0015 NEW)

add_executable(myfs myfs.c)
add_executable(test test.c mystdio.c myfs.c)
add_executable(mystdio mystdio.c)
add_executable(main main.c mystdio.c myfs.c)
