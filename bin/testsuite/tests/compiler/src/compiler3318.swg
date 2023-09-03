#global testerror "Err0116"
struct Duration
{
    val: f32
}

impl Duration
{
    #[Swag.Implicit]
    mtd opAffect(v: s32)
    {
        @assert(v == 6)
    }

    #[Swag.Implicit]
    mtd opAffect(v: f32)
    {
        @assert(v == 6)
    }
}

#test
{
    var x: Duration = 4
}
