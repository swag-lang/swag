//#[Swag.PrintBc]
#test
{
    var g   = 0
    const x = "66"
    switch x
    {
    case "11":
        @assert(false)
    case "66":
        g = 1; break
    default:
        @assert(false)
    }

    @assert(g == 1)
}
