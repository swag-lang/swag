#global testerror

func toto(f: f32)
#selectif
{
    return false
}
{
}

#test
{
    toto(0)
}