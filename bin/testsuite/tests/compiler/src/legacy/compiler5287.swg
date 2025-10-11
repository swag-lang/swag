#test
{
    struct RGB { r = 1, g = 2, b = 3 }

    var rgb: [4] RGB
    @init(rgb)(50, 60, 70)
    @assert(rgb[3].r == 50)
    @assert(rgb[3].g == 60)
    @assert(rgb[3].b == 70)
}
