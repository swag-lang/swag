struct(T) Array {}

struct(T) Toto {}

impl Toto
{
    mtd toto()
    {
        var x: Array's32
        @assert(#typeof(Array).flags & .Generic ? true : false)
    }
}

#test
{
    var toto: Toto's32
    toto.toto()
}
