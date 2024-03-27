#global testerror "Err0085"

func toto(f: f32)
    #validif
    {
        return false
    }
{
}

#test
{
    toto(0)
}
