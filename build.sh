#!/bin/bash

DEPENDENCIES="";
CFLAGS="-Wall -Wextra -pedantic"; #`pkg-config --libs $DEPENDENCIES`";
CLIBS="" #`pkg-config --cflags $DEPENDENCIES`";
CC="gcc";
set -xe

function build {
    $CC src/main.c $CFLAGS -o love $CLIBS;
}
build;
