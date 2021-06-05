#[Swag.enumflags]
enum RGB: u32 { R; G; B; }

#test
{
    var x: RGB = R | G
    #assert @typeof(RGB) == RGB
    #assert @kindof(RGB) == u32
//    @assert(x == .R)
}