enum RGB { R = 1, G, B }
enum RGB1 { R = 10, G, B }

func toto(x: bool, rgb: RGB, rgb1: RGB1)
{
    @assert(cast(s32) rgb == 1)
    @assert(cast(s32) rgb1 == 10)
    @assert(rgb == .R)
    @assert(rgb1 == .R)
}

#test
{
    let x = true
    x.toto(.R, .R)
}
