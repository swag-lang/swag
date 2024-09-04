#global testerror "Err0075"

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