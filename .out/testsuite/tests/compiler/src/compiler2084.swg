struct AA
{
    x: u32 = 444
    y: {a: u32, b: u32} = @{555, 666}
}

//#[Swag.printbc]
#test
{
    var tt: AA
    @assert(tt.x == 444)
    @assert(tt.y.a == 555)
    @assert(tt.y.b == 666)
}