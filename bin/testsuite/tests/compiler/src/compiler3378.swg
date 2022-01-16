//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
#test
{
    a := true
    b := true
    c := false
    if !a or (!b and !c)
        @assert(false)
}