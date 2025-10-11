#global testerror "Err0711"

#[Swag.Macro]
func assert(expr: #code bool)
{
    //@print("before")
    #inject(expr)
    //@print("after")
}

var G = 0
func xx()->bool
{
    G = 1
    return true
}

//#[Swag.PrintBc]
#test
{
    assert(xx())
    @assert(G == 1)
}
