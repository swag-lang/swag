#global testerror "Err0329"

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