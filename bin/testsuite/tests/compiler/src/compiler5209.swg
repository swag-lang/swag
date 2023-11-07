#test
{
    var x: Swag.IAllocator
    let y = @kindof(x)
    if y
        @print(y.name)
}