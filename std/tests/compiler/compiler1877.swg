//#[swag.printbc]
#test
{
    //tcf_toto16(456,3.14)
    ptr := &tcf_toto16
    ptr(456,3.14)

    ptr1 := &tcf_toto5
    @assert(ptr1(10) == 10)
}