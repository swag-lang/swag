struct AA {}

impl AA
{
    #[Swag.Overload]
    func opEquals(me, x: s32) => x == 6
    #[Swag.Overload]
    func opEquals(me, x: string) => x == "toto"
}

#test
{
    var x: AA
    switch x
    {
    case 4:
        @assert(false)
    case 5:
        @assert(false)
    case "toto":
        break
    case 6:
        break
    default:
        @assert(false)
    }
}
