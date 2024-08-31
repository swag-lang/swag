#global testerror "Err0352"

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