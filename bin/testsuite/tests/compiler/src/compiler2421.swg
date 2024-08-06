#global testerror "Err0077"

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