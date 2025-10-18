#global testerror "Err0514"

func doIt(fn: func()) {}

#test
{
    for 2
    {
        doIt(func() { break; })
    }
}
