#global skip
struct AA {}

impl AA
{
    #[Swag.Inline]
    func opEquals(using self, x: s32) => x == 6
}

#test
{
    var x: AA
    switch x
    {
    case 6:  break
    default: @assert(false)
    }
}
