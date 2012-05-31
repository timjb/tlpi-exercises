#!/bin/sh

function assertEqual {
  if [ "$1" != "$2" ]; then
    echo "Expected \"$1\" to be \"$2\""
    exit 1
  fi
}

CURR_DIR=$(pwd)

function my_pwd {
  $CURR_DIR/getcwd_clone $@
}

cd /usr/bin
assertEqual "$(my_pwd)" "/usr/bin"
cd /usr/share
assertEqual "$(my_pwd)" "/usr/share"
cd /etc
assertEqual "$(my_pwd)" "/etc"
