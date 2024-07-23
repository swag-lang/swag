#global testerror "Err0085"

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