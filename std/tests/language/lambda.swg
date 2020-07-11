#test
{
    // A lambda is just a pointer to a function
    {
        func myFunction0() {}
        func myFunction1(x: s32) => x * x

        var ptr0: func() = &myFunction0
        ptr0()

        ptr1 := &myFunction1
        @assert(myFunction1(2) == 4)
        @assert(ptr1(2) == 4)
    }

    // A lambda can be null
    {
        var lambda: func()->bool
        @assert(lambda == null)
    }

    {
        /*typealias callback: func(s32)->s32
        func toDo(value: s32, ptr: callback)->s32
        {
            return ptr(value)
        }

        func square(x: s32) => x * x*/
    }
}