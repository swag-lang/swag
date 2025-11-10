struct Array {}

impl Array
{
    #[Swag.Implicit]
    mtd opAffect(v: const [..] f32)
    {
        @assert(v[0] == 1)
        @assert(v[1] == 15)
        @assert(v[2] == 23)
        @assert(@countof(v) == 3)
    }
}

alias Vector = Array

func toto(x: Vector) {}

//#[Swag.PrintBc]
#test
{
    toto([1, 15, 23])
}
