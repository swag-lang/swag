struct AA
{

}

impl AA
{
    func opEquals(using self, x: s32) => x == 6
}

#test
{
    var x: AA
    switch x
    {
    case 4:     @assert(false)
    case 5:     @assert(false)
    case 6:     break
    default:    @assert(false)
    }
}
