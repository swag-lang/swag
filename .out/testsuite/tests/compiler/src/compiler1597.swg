var G: s32

func(op: string, T) toto(x, y: T)->T
{
    #ast "return x" ~op~ "y"
}

#test
{
    var x, y: s32
    @assert(toto'("+", s64)(10, 5) == 15)
    @assert(toto'("+", f32)(1.5, 1.5) == 3.0)
}