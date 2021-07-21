#global skip // remove typeset from language the 21 july 2021
var G: s32

typeset AA
{
    T  {x: s32}
    T1 {s32, y: s32}
    T2 {}
}

func toto(param: AA)
{
    switch @kindof(param)
    {
    case AA.T:  v := cast(AA.T) param; G += v.x
    case AA.T1: v := cast(AA.T1) param; G += v.item0; G += v.y
    case AA.T2: G += 10
    }
}

//#[Swag.PrintBc]
#test
{
    var v: AA

    v = AA.T{1}
    toto(v)
    v = AA.T1{2, 3}
    toto(v)
    v = AA.T2{}
    toto(v)
    @assert(G == 16)
}