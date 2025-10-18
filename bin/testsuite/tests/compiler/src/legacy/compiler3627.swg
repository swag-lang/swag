#global testerror "Err0389"

struct C
{
    length: s32
}

var g_RD: [40] C

#test
{
    for i in g_RD.length
    {
    }
}
