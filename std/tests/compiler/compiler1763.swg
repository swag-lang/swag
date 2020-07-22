//#[swag.printbc]
#test
{
    var x: [..] s64 = [10, 20]
    ptr := &x[1] - 1
    @print(ptr[0])
}