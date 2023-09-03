#global testerror "Err0632"

func do(fn: func())
{
}

#test
{
    loop 2
    {
        do(func() { break; })
    }
}