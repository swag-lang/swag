#global testerror "Err0444"

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