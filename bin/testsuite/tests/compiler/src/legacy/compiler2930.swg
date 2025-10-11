//#global testerror

#test
{
    let x = 10
    switch x
    {
    case 10 to 0:
        break
    default:
        @assert(false)
    }
}
