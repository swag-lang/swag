//
//#global testerror

var G = 0

#[swag.macro]
func cstr(x: string)->string
{
    defer G = 1
    return "toto"
}

//#[swag.printbc]
#test
{
    v := cstr("toto")
    @assert(G == 1)
    @assert(v == "toto")
}