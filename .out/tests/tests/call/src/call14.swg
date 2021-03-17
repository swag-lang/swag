using tests_foreign

func cb()->s64
{
    return 666
}

func cb1(v: s64)->string
{
    @assert(v == 444)
    return "toto"
}

func cb2(v: f32)->string
{
    @assert(v == 555.0)
    return "toto"
}

#test
{
    @assert(tcf_callret(&cb) == 666)
    @assert(tcf_callret1(444, &cb1) == "toto")
    @assert(tcf_callret2(555.0, &cb2) == "toto")
}