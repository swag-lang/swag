#global testerror "Err0076"

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