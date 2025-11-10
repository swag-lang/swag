struct(T) Array { x, y, z: T }

impl Array
{
    #[Swag.Implicit]
    mtd opAffect(v: const [..] T)
    {
        @assert(v[0] == 1)
        @assert(v[1] == 15)
        @assert(v[2] == 23)
        @assert(@countof(v) == 3)
        me.x = v[0]
        me.y = v[1]
        me.z = v[2]
    }
}

alias Vector = Array'f32

func toto(s: Vector)
{
    @assert(s.x == 1)
    @assert(s.y == 15)
    @assert(s.z == 23)
}

//#[Swag.PrintBc]
#test
{
    toto([1, 15, 23])
}
