#test
{
    var x: any = "t"
    switch x
    {
    case s32:       @assert(false)
    default:        @assert(false)
    case string:    break
    }
}