PROJECT_ROOT=`dirname "$(realpath "$0")"`

(cd "$PROJECT_ROOT/build"; cmake ..; make)
