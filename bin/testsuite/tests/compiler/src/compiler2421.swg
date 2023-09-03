#global testerror "Err0004"

func toto(f: f32)
#validif
{
    return false
}
{
}

#test
{
    toto(0)
}