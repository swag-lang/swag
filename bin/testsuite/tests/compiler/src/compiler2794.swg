#[Swag.EnumFlags]
enum RGB: u32 { R, G, B }

#test
{
    var x: RGB = R | G
    #assert(#typeof(RGB) == RGB)
    #assert(#typeof(RGB).rawType == u32)
    //    @assert(x == .R)
}
