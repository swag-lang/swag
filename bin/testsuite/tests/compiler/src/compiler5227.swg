struct AA
{
    s    = "toto"
}

//#[Swag.PrintBc]
#test
{
    var arr1: [4, 4] AA
    let addr  = cast([*] AA) @dataof(arr1)
    let value = #ref &addr[0]
    @assert(value.s == "toto")
}
