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
    func init(self, x: s32)->s32
    func close(self)->s32
    func data(self)
}

impl I for Base
{
    mtd impl init(param: s32) => param
    mtd impl close()->s32 => 2
    mtd impl data() {}
}

impl I for BB
{
    mtd impl close()->s32 => 20
}

#test
{
    var x: BB
    var ptr = cast(I) x
    @assert(ptr.init(1) == 1)
    @assert(ptr.init(10) == 10)
    @assert(ptr.close() == 20)
}