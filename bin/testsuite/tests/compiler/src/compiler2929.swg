#global testerror "Err0342"

#test
{
    var x = 10
    switch x
    {
    case "0" to 10: break
    default:    @assert(false)
    }
}