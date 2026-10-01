#include "meataxe.h"

/* Generate tables for all prime powers supported by the small-field kernel. */
int main(int argc, const char **argv)
{
    int q, p, n;
    MtxApplication_t *app = AppAlloc(NULL, argc, argv);
    if (app == NULL)
        return 1;
    for (q = 2; q <= 256; ++q)
    {
        for (p = 2; q % p != 0; ++p)
            ;
        n = q;
        do { n /= p; } while (n % p == 0);
        if (n == 1 && FfSetField(q) != 0)
            return 1;
    }
    AppFree(app);
    return 0;
}
