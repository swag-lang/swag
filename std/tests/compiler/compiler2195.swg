#module "compiler2195"
#unittest error

var G = 0

#[swag.macro]
func cstr(x: string)->string
{
    defer G = 1
}

//#[swag.printbc]
#test
{
    v := cstr("toto")
    @print(v)
    @assert(G == 1)
}