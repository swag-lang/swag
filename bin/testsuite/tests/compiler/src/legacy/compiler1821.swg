//#[Swag.PrintBc]
#test
{
    var a = [1, 2, 3, 4]
    var s = a[0 to 3]
    @assert(&a[0] == &s[0])
}
