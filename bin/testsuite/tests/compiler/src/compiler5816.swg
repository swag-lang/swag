// 303379
// 303283
// 303031
// 302549
// 302393
// 302369
// 302357
// 299882
// 299419

var a: ^bool

//#[Swag.PrintBc]
func toto(x: s32)->bool
{
    var i = 0
    while a[x] and a[x + 1]:
        i += 41
    return true
}

#test
{
    var b: [10] bool
    a = b
    @assert(toto(0) == true)
}