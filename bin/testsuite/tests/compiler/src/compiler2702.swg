#global testerror "Err0350"

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