#global testerror "Err0403"

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
