#test
{
    myVar := 0
    #inline
    {
        alias myVar = #up myVar
        myVar += 1
    }

    @assert(myVar == 1)
}