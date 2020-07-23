#[swag.printbc]
#test
{
    x := [1, 2, 3]
    ptr := &x[0]
    ptr += 3
    ptr -= 1
    if false
    @print(cast(s64) deref ptr)
}