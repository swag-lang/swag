struct TOTO
{
    x: s32
}

struct TITI
{
    base: TOTO
}

#test
{
    let p: TITI
    //p.x = 666
}