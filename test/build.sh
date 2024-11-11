
mkdir -p ../target/test
gcc -g  -DDEBUG -I../src/ ../src/list.c assert.c list.test.c -o ../target/test/list.test
gcc -g  -DDEBUG -I../src/ ../src/log_view_header.c ../src/list.c assert.c log_view_header.test.c -o ../target/test/log_view_header.test -lncurses
gcc -g  -DDEBUG -I../src/ ../src/log.c ../src/logger.c assert.c log.test.c -o ../target/test/log.test
gcc -g  -DDEBUG -I../src/ ../src/string_view.c assert.c string_view.test.c -o ../target/test/string_view.test

./../target/test/list.test "$@"
./../target/test/log_view_header.test "$@"
./../target/test/log.test "$@"
./../target/test/string_view.test "$@"
./../target/test/command_history.test "$@"
