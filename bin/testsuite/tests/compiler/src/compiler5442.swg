#global testerror "Err0157"

func toto(x: &s32)
{
    x = 0
}

#test
{
    let a = 0
    a.toto()
}