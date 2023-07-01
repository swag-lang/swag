struct A
{
    x: s32
}

interface I
{
    x: func(self)
}

impl I for A
{
    mtd impl x(){}
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var v: *A
    x := cast(I) cast(*A) v
    t := @kindof(x)
    @assert(t and @countof(t.fullname))
}