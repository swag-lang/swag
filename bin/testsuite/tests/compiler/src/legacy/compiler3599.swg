enum RGB { R, G, B }
enum RGB1 { G }

struct X {}

impl X
{
    mtd toto(k: RGB, k1: RGB1) {}

    mtd toto1()
    {
        me.toto(.R, .G)
    }
}
