#test
{
    // #inline is used to create an inlined separated scope
    {
        myVar := 0
        #inline
        {
            myVar := 5
        }

        @assert(myVar == 0)
    }

    // You can reference a variable outside the #inline scope with a backtick
    {
        myVar := 0
        #inline
        {
            `myVar += 1
        }

        @assert(myVar == 1)
    }

    // You can alias an out of scope variable for easy referencing
    {
        myVar := 0
        #inline
        {
            alias myVar = `myVar
            myVar += 1
            myVar += 2
        }

        @assert(myVar == 3)
    }
}