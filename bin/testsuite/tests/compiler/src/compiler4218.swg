//#global skip
struct AA {}

impl AA
{
    #[Swag.Inline]
    func opEquals(using self, x: string) => x == "6"
}

#test
{
    var x: AA
    var G = 0

    switch x
    {
    case "6":   G = 1; break
    default:    @assert(false)
    }

    @assert(G == 1)
}
