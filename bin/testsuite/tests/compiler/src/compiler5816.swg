var a: ^bool

#[Swag.PrintBc]
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
    @print(toto(0))
}