//#[Swag.PrintBc]
#test
{
    const x: any = "t"
    switch x
    {
    case s32:
        @assert(false)
    case string:
        break
    default:
        @assert(false)
    }
}
