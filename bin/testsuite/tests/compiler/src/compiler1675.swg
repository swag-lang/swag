//#[Swag.PrintBc]
#run
{
    let s   = 1
    var res = false
    switch s
    {
    case 1:
        fallthrough
    case 2:
        res = true
    }
    @assert(res)
}
