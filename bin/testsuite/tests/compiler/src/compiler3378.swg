//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
#test
{
    var a = true
    var b = true
    var c = false
    if !a or (!b and !c)
        @assert(false)
}