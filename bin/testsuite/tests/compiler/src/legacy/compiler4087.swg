//#global testerror
struct BB
{
    v: s32
}

struct AA
{
    bb: BB
}

const APP0info = AA{{42}}
#test
{
    @assert(APP0info.bb.v == 42)
    #assert(APP0info.bb.v == 42)
}
