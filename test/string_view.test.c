#include "assert.h"
#include "string_view.h"

String_View svs[6];
int counter = 0;

static void test_chop_by_line() {

    const char* test = "This is a example of tests";

    String_View sv = build_from_char(test, strlen(test) + 1);

    String_View nl;
    nl = get_next_line(&sv, 5);
    svs[counter++] = nl;
    assert(strncmp(nl.text, "This ", nl.size) == 0);
    nl = get_next_line(&sv, 5);
    svs[counter++] = nl;
    assert(strncmp(nl.text, "is a ", nl.size) == 0);
    nl = get_next_line(&sv, 5);
    svs[counter++] = nl;
    assert(strncmp(nl.text, "examp", nl.size) == 0);
    nl = get_next_line(&sv, 5);
    svs[counter++] = nl;
    assert(strncmp(nl.text, "le of", nl.size) == 0);
    nl = get_next_line(&sv, 5);
    svs[counter++] = nl;
    assert(strncmp(nl.text, " test", nl.size) == 0);
    nl = get_next_line(&sv, 5);
    svs[counter++] = nl;
    assert(strncmp(nl.text, "s", nl.size) == 0);
}

int main(int argc, char** argv) {

    RUN_FILE(argc, argv, {
        RUN_TEST(test_chop_by_line);
    });

    for(int i = 0 ; i < counter ; i++) {
        printf(String_View_Fmt"\n", String_View_Arg(svs[i]));
    }
}