#[swag.printbc]
#test
{
    var x: [] string = ["1", "2", "3", "4"]
    init(&x[1])
    @print(x[1])
}