//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
#test
{
    let a = true
    let b = true
    let c = false
    if !a or (!b and !c) do
        @assert(false)
}
