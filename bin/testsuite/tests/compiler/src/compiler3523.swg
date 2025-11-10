struct(T) Array {}

impl Array
{
    mtd opAffect(v: const [..] T)
    {
        @assert(v[0] == 1)
        @assert(v[1] == 15)
        @assert(v[2] == 23)
        @assert(@countof(v) == 3)
    }
}

alias Vector = Array'f32

func toto(x: Vector) {}

//#[Swag.PrintBc]
#test
{
    toto(cast(Vector) [1, 15, 23])
}
