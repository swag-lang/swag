#global testerror "Err0184"

func toto(x: &s32)
{
    x = 0
}

#test
{
    let a = 0
    a.toto()
}