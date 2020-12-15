#test
{
    myVar := 0
    #inline
    {
        alias v = `myVar
        //`myVar += 1
        v += 1
    }

    @assert(myVar == 1)
}