struct Base
{
    x: s32 = 555
}

struct BB
{
    x: [4] bool
    using base: Base
}

interface I
{
    init:  func(self, s32)->s32
    close: func(self)->s32
}

impl I for Base
{
    mtd init(param: s32) => param
    mtd close()->s32 => 2
}

impl I for BB
{
    mtd close()->s32 => 20
}

#test
{
    var x: BB
    ptr := cast(I) x
    @assert(ptr.init(1) == 1)
    @assert(ptr.init(10) == 10)
    @assert(ptr.close() == 20)
}