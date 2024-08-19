#global testerror "Err0063"

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