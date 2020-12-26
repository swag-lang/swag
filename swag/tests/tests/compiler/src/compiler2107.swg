typeset AA
{
    B{s32}
}

func log(v: *AA)
{
    @assert(@kindof(:v) == AA.B)
}

#test
{
    var x: AA = AA.B{5}
    log(&x)
}
