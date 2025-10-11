#[Swag.Macro]
func assert(expr: #code void)
{
    //@print("before")
    #inject(expr)
    //@print("after")
}

var G = 0
func xx()
{
    G = 1
    return
}

//#[Swag.PrintBc]
#test
{
    assert(xx())
    @assert(G == 1)
}
