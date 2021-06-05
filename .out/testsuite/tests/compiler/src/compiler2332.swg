#test
{
    myVar := 0
    #inline
    {
        alias myVar = `myVar
        myVar += 1
    }

    @assert(myVar == 1)
}