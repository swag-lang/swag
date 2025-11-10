var a: [*] bool

//#[Swag.PrintBc]
func toto(x: s32)->bool
{
    var i = 0
    while a[x] and a[x + 1] do
        i += 41
    return true
}

#test
{
    var b: [10] bool
    a = b
    @assert(toto(0) == true)
}
