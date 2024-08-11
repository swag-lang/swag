#global testerror "Err0062"

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