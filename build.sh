#!/bin/bash

DEPENDENCIES="";
CFLAGS="-Wall -Wextra"; #`pkg-config --libs $DEPENDENCIES`";
CLIBS="" #`pkg-config --cflags $DEPENDENCIES`";
CC="gcc";

function build {
    $CC src/main.c $CFLAGS -o love $CLIBS;
}
build;
