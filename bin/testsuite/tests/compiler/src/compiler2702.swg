#global testerror "Err0311"

func doIt(fn: func())
{
}

#test
{
    for 2
    {
        doIt(func() { break; })
    }
}