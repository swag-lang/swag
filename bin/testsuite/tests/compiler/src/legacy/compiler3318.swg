//#global testerror "Err0024"
struct Duration
{
    val: f32
}

impl Duration
{
    #[Swag.Implicit]
    #[Swag.Overload]
    mtd opAffect(v: s32)
    {
        @assert(v == 4)
    }

    #[Swag.Implicit]
    #[Swag.Overload]
    mtd opAffect(v: f32)
    {
        @assert(v == 4)
    }
}

#test
{
    var x: Duration = 4
}
