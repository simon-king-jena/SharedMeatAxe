#include "meataxe.h"
#include <stdio.h>
#include <string.h>

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
    if (app != NULL)
        AppFree(app);
    return 0;
}
