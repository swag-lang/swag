//#global testerror
//#[Swag.PrintBc]
#test
{
    {
        var x: [2] bool = true
        @assert(x[0] == true)
        @assert(x[1] == true)
    }
    {
        var x: [2] s32 = 62
        @assert(x[0] == 62)
        @assert(x[1] == 62)
    }
}
