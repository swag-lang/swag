#global skip // remove typeset from language the 21 july 2021
typeset AA
{
    B{a:s32}
}

func log(v: *AA)
{
    @assert(@kindof(dref v) == AA.B)
}

#test
{
    var x: AA = AA.B{5}
    log(&x)
}
