struct AA
{
    x: u32 = 666
    y: {a: u32, b: u32} = @{666, 666}
}

//#[swag.printbc]
#test
{
    var tt: AA
    @assert(tt.x == 666)
    @print(cast(s64) @sizeof(tt))
    @print(cast(s64) tt.y.a)
}