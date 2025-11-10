struct A { x, y: u8 }

func toto(x: s32)
{
    @assert(x == -1)
}

#test
{
    var a: A = {1, 10}
    var b: A = {1, 20}

    toto(a.y <=> b.y)
}
