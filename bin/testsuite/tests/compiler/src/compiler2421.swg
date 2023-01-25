#global testerror

func toto(f: f32)
#selectifonce
{
    return false
}
{
}

#test
{
    toto(0)
}