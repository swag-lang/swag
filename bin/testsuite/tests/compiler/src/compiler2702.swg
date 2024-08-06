#global testerror "Err0433"

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