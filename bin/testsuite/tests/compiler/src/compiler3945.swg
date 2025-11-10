struct V { x, y: s32 }

var G: V{1, 10}
func toto()->const &V
{
    return &G
}

//#[Swag.PrintBc]
#test
{
    let x: const &V = toto()
    @assert(x.x == 1)
    @assert(x.y == 10)
    G.x = 4
    @assert(x.x == 4)
}
