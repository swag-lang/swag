#global testerror

#test
{
    x := 10
    switch x
    {
    case "0"..10: break
    default:    @assert(false)
    }
}