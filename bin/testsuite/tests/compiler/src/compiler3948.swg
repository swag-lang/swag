struct V
{
    x, y: s32
}

var G: V{1, 10}

impl V
{
    mtd(op: string) opAssign(val: s32)
    {
        x, y += val
    }
}

func toto()->ref V
{
    return &G
}

//#[Swag.PrintBc]
#test
{
    var x = toto()
    x += 1
    @assert(x.x == 2)
    @assert(x.y == 11)
}
