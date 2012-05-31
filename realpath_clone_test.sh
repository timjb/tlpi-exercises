#!/bin/sh

function assertEqual {
  if [ "$1" != "$2" ]; then
    echo "Expected \"$1\" to be \"$2\""
    exit 1
  fi
}

function assertFail {
  $@ >/dev/null 2>/dev/null
  if [ $? == "0" ]; then
    echo "Expected \"$@\" to fail"
    exit 1
  fi
}

CURR_DIR=$(pwd)

function my_realpath {
  $CURR_DIR/realpath_clone $@
}

cd /tmp
[ -d lorem_ipsum ] && rm -R lorem_ipsum
mkdir lorem_ipsum
cd lorem_ipsum
touch a
ln -s a b
ln -s ../lorem_ipsum/a c
ln -s /tmp/lorem_ipsum/c d

assertEqual "$(my_realpath a)" "/tmp/lorem_ipsum/a"
assertEqual "$(my_realpath b)" "/tmp/lorem_ipsum/a"
assertEqual "$(my_realpath c)" "/tmp/lorem_ipsum/a"
assertEqual "$(my_realpath d)" "/tmp/lorem_ipsum/a"
assertEqual "$(my_realpath /tmp/lorem_ipsum/a)" "/tmp/lorem_ipsum/a"
assertEqual "$(my_realpath ./a)" "/tmp/lorem_ipsum/a"
assertEqual "$(my_realpath ././a)" "/tmp/lorem_ipsum/a"
assertEqual "$(my_realpath ./../lorem_ipsum/a)" "/tmp/lorem_ipsum/a"
assertEqual "$(my_realpath ../../../../../../../tmp/lorem_ipsum/a)" "/tmp/lorem_ipsum/a"
assertFail my_realpath nosuchfile
