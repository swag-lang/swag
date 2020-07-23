//#[swag.printbc]
#test
{
    x := [1's8, 2's8, 3's8]
    ptr := &x[0]
    ptr += 3
    ptr -= 1
    @print(cast(s64) deref ptr)
}