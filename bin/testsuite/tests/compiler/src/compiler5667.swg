//#[Swag.PrintBc]
#test
{
    let x = 7
    let y = 10

    switch x
    {
    case 6 where y == 10:
        @assert(false) // This will not be executed.
    case 7 where y == 9:
        @assert(false) // This will not be executed.
    default where y == 5:
        @assert(false) // This will not be executed since the previous default matched.
    default where y == 10:
        break // This will execute since 'y' is 10.
    default:
        @assert(false) // This will not be executed since the previous default matched.
    }
}
