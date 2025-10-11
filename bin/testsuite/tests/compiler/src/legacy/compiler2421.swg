#global testerror "Err0078"

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
