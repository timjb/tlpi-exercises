#!/bin/sh

EXERCISES_DIR="$(pwd)"

function echoDo {
  echo "\$" $@ 1>&2
  $@
}

# we redirect stderr to stdout because ordering is important
# to comprehend the output of this script

echo "\$ cd /tmp" 1>&2
cd /tmp
echo "\$ [ -d dolores ] && rm -R dolores" 1>&2
[ -d dolores ] && rm -R dolores
echoDo mkdir dolores
echoDo cd dolores
echoDo mkdir adir
echoDo mkdir bdir
echo "\$ $EXERCISES_DIR/inotify_recursive adir bdir &" 1>&2
$EXERCISES_DIR/inotify_recursive adir bdir 1>&2 &
INOTIFY_RECURSIVE_PID=$!
echo "\$ INOTIFY_RECURSIVE_PID=\$!" 1>&2
sleep 1
echoDo touch adir/lorem
echoDo mkdir bdir/subdir
echoDo touch bdir/subdir/file1
echoDo rm adir/lorem
echoDo mv bdir/subdir adir
echo "\$ echo \"test\" > adir/subdir/file1" 1>&2
echo "test" > adir/subdir/file1
echoDo mv adir/subdir .
echoDo touch subdir/file1
echo "\$ kill \$INOTIFY_RECURSIVE_PID" 1>&2
kill $INOTIFY_RECURSIVE_PID
sleep 1 # make sure to kill child process