typeset AA
{
    B{s32}
}

func log(v: *AA)
{
    @print(@kindof(:v).name)
}

#test
{
    var x: AA = AA.B{5}
    //log(&x)
}
