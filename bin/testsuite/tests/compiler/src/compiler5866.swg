#global testerror "Err0349"

struct V2 {x, y: s32}

#test
{
    var a: s32

    var f: func||()
    f = func|using a|()
    {
        @assert(x == 1)
    }

    f()
}