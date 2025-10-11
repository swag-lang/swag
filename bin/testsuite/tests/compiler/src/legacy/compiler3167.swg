struct AAA
{
    v    = 666
}

func toto(xxx = AAA{})
{
    @assert(xxx.v == 666)
}

#test
{
    toto()
}
