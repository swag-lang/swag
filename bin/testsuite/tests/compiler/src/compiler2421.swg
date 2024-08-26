#global testerror "Err0080"

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