#global testerror "Err0579"

struct(T) Arr {}

impl Arr
{
    #[Swag.Overload]
    mtd sort() {}
    #[Swag.Overload]
    mtd sort(cb: func||(T, T)->s32) {}
}

#test
{
    let a: Arr's32
    a.sort(func(x, y) => 0)
}
