struct AA
{
    s    = "toto"
}

//#[Swag.PrintBc]
#test
{
    var arr: [4] AA
    foreach v in arr
    {
        @assert(v.s == "toto")
    }

    var arr1: [4, 4] AA
    foreach v in arr1
    {
        @assert(v.s == "toto")
    }
}
