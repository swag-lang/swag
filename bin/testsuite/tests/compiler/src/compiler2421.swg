#global testerror "Err0083"

func toto(f: f32)
where
{
    return false
}
{
}

#test
{
    toto(0)
}