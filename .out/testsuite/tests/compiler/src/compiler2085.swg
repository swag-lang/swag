struct BB
{
    x, y: u32 = 444
}

struct AA
{
    x: u32 = 444
    y: BB = @{555, 666}
}

//#[Swag.printbc]
#test
{
    var tt: AA
    @assert(tt.x == 444)
    @assert(tt.y.x == 555)
    @assert(tt.y.y == 666)

    //zz := @{1, 2}
    //zz.a = 0
}