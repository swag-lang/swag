#[swag.printbc]
#test
{
    var x: [..] s64 = [10, 20]
    ptr := &x[0]

    @print(ptr[1])
}