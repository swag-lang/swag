#global testerror "Err0712"

const A = false

struct E
{
    #if A
    {
        x: s32
    }
    #else
    {
        y: s32
    }
}

#test
{
    var pt: E
    @assert(pt.x == 0)
}