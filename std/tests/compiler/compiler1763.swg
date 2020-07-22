#[swag.printbc]
#test
{
    var x: [..] s64 = [1, 2]
    x[1] = 10
    @print(x[1])
}