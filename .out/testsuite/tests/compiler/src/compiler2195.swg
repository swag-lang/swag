//
//#global testerror

var G = 0

#[Swag.macro]
func cstr(x: string)->string
{
    defer G = 1
    return "toto"
}

//#[Swag.PrintBc]
#test
{
    v := cstr("toto")
    @assert(G == 1)
    @assert(v == "toto")
}