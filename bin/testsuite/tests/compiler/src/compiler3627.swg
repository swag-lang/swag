#global testerror "Err0440"

struct C
{
    length: s32
}

var g_RD: [40] C

#test
{
    loop i in g_RD.length
    {
    }
}