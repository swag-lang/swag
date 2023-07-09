#test
{
    var x = 0'u8
    var y = 255'u8
    var cpt = 0
    loop x..y cpt += 1
    @assert(cpt == 256)
}