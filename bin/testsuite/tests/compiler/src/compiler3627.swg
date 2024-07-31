#global testerror "Err0552"

struct C
{
    length: s32
}

var g_RD: [40] C

#test
{
    loop i: g_RD.length
    {
    }
}