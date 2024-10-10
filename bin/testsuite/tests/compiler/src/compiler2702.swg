#global testerror "Err0322"

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