var g: [4 * 4] s32 = 5

#test
{
    #[Swag.Mixin]
    func mp(x0, y0: u64)->&s32 => &g[0] + (x0 + y0 * 4)

    for x in 0 to 3'u64
    {
        for y in 0 to 3'u64
        {
            //@print(mp(x,y), " ")
            //@assert(mp(x, y) == 5)
            @assert(g[x + y * 4] == 5)
            //@print(mp(x, y), " ", g[x + y*4], "\n")
        }
    }
}
