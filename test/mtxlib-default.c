#include "meataxe.h"
#include <stdio.h>
#include <string.h>

/* Ordinary output must stay in the working directory for every table setting. */
static int CheckOutput(void)
{
    const char *name = "ordinary-output.txt";
    const char *text = "ordinary output\n";
    char buf[80];
    FILE *f = SysFopen(name, FM_CREATE | FM_TEXT);
    if (f == NULL)
        return 1;
    if (fputs(text, f) == EOF)
    {
        fclose(f);
        return 1;
    }
    if (fclose(f) != 0)
        return 1;

    /* Use stdio to verify the actual location, independently of SysFopen(). */
    f = fopen(name, "r");
    if (f == NULL)
    {
        fprintf(stderr, "Ordinary output was not created in the working directory\n");
        return 1;
    }
    if (fgets(buf, sizeof(buf), f) == NULL || strcmp(buf, text) != 0)
    {
        fprintf(stderr, "Unexpected ordinary output contents\n");
        fclose(f);
        return 1;
    }
    fclose(f);
    return remove(name) != 0;
}

int main(int argc, const char **argv)
{
    MtxApplication_t *app = NULL;
    if (argc < 2)
        return 1;
    /* With just an expected path, test a library client without AppAlloc. */
    if (argc > 2)
        app = AppAlloc(NULL, argc - 1, argv + 1);
    else
        MtxInitLibrary();
    if (strcmp(MtxLibDir, argv[1]) != 0)
    {
        fprintf(stderr, "Expected %s, got %s\n", argv[1], MtxLibDir);
        return 1;
    }
    if (CheckOutput() != 0)
        return 1;
    if (app != NULL)
        AppFree(app);
    return 0;
}
