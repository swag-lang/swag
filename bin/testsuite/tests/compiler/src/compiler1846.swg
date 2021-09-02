//#[Swag.PrintBc]
#test
{
    //func myFunction0() {}
    func myFunction1(x: s32) => x * x

    //var ptr0: func() = &myFunction0
    //ptr0()

    ptr1 := &myFunction1
    //@assert(myFunction1(2) == 4)
    @assert(ptr1(2) == 4)
}