//#global skip
struct X {}
struct Y { idx: u64 }

impl X
{
    mtd opIndex(x: u64)->Y
    {
        return {x}
    }
}

impl Y
{
    mtd opIndex(x: u64)->u64
    {
        return me.idx + x * 10
    }
}

//#[Swag.PrintBc]
#test
{
    var x: X
    @assert(x[0][1] == 10)
    @assert(x[1][1] == 11)
    @assert(x[2][2] == 22)
}
