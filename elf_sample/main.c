typedef int (*puts_t)(const char *s);
/*
puts_t xputs;
puts_t *pxputs = &xputs;


void _start(puts_t p)
{
    *pxputs = p;

    (*pxputs)("Hello world from ELF!");
}
*/
#define NULL ((void*) 0)

static puts_t write_text_output = NULL;
// Entry
void _start(puts_t p){
    write_text_output = p;
    write_text_output("Hello world from ELF!\n");
}