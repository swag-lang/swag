#global testerror

func do(fn: func())
{
}

#test
{
    loop 2
    {
        do(@() { break; })
    }
}