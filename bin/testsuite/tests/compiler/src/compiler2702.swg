#global testerror "Err0455"

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