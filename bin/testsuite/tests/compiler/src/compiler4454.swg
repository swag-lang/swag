//#global testerror
//#[Swag.PrintBc]
#test
{
    {
        var x: [2] string = "true"
        @assert(x[0] == "true")
        @assert(x[1] == "true")
    }
}
