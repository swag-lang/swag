// 303379
// 303283
// 303031

var a: ^bool

//#[Swag.PrintBc]
func toto(x: s32)->bool
{
    if a[x] and a[x + 1]:
        return false
    return true
}

#test
{
    var b: [10] bool
    a = b
    @assert(toto(0) == true)
}