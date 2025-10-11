#test
{
    var rgb: [4] s32
    @init(rgb)(50)
    @assert(rgb[0] == 50)
    @assert(rgb[1] == 50)
    @assert(rgb[2] == 50)
    @assert(rgb[3] == 50)
}
