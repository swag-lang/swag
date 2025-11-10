#test
{
    let x   = 0'u8
    let y   = 255'u8
    var cpt = 0
    for x to y do
        cpt += 1
    @assert(cpt == 256)
}
