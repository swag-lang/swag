//#[Swag.PrintBc]
#test
{
    var x = 256
    if x do
        @assert(true)
    else do
        @assert(false)

    x = 0
    if !x do
        @assert(true)
    else do
        @assert(false)
}
