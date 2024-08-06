#global testerror "Err0399"

func doIt(fn: func())
{
}

#test
{
    loop 2
    {
        doIt(func() { break; })
    }
}