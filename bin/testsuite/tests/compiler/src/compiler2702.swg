#global testerror "Err0632"

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