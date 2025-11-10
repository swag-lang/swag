struct(T) Array {}

struct(T) Toto {}

impl Toto
{
    mtd toto(x: const *Swag.TypeInfoStruct)
    {
        @assert(x.fromGeneric == #typeof(Array))
        @assert(x.fromGeneric == Array)
    }
}

#test
{
    var x: Toto's32
    x.toto(Array's32)
}
