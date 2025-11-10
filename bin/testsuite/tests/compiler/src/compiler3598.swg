enum RGB { R, G, B }
enum RGB1 { R1, G1, B1 }

struct X {}

impl X
{
    mtd toto(k: RGB, k1: RGB1) {}

    mtd toto1()
    {
        me.toto(.R, .R1)
    }
}
