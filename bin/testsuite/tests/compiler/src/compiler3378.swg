//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
#test
{
    let a = true
    let b = true
    let c = false
    if !a or (!b and !c):
        @assert(false)
}