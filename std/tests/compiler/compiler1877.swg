func thisFunc(x: s64)
{
    @print(x)
    @assert(x == 555)
}

//#[swag.printbc]
#test
{
    //tcf_toto16(456,3.14)
    /*ptr := &tcf_toto16
    ptr(456,3.14)

    ptr1 := &tcf_toto5
    @assert(ptr1(10) == 10)*/

    ptr2 := &tcf_toto17
    ptr2(555, &thisFunc)
}