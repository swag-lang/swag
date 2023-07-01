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
    init:  func(self)->s32
    data:  func(self)->s32
    close: func(self)->s32
}

impl I for Base
{
    mtd impl init()->s32 => 1
    mtd impl close()->s32 => 2
    mtd impl data()->s32 => 3
}

impl I for BB
{
    mtd impl close()->s32 => 20
}

#test
{
    var x: BB
    ptr := cast(I) x
    @assert(ptr.init() == 1)
    @assert(ptr.data() == 3)
    @assert(ptr.close() == 20)
}