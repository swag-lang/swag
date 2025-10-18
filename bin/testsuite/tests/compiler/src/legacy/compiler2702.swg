#global testerror "Err0555"

func doIt(fn: func()) {}

#test
{
    for 2
    {
        doIt(func() { break; })
    }
}
