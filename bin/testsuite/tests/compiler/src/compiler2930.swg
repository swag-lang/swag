//#global testerror

#test
{
    let x = 10
    switch x
    {
    case 10..0: break
    default:    @assert(false)
    }
}