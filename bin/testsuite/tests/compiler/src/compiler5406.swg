//#global skip

struct(T) A
{
}

var g = 6'u32

impl A
{
    #[Swag.Overload, Swag.Inline]
    mtd opIndexAffect(i: u64, v: T)
    {
        @assert(i == 6)
        @assert(v == 66)
    }
}

func toto()->&u32
{
    return &g
}

#[Swag.Optim("", false)]
#[Swag.Safety("", false)]
#test
{
    var a: A'u32
    a[toto()] = 66
}