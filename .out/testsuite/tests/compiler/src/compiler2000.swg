//#[Swag.PrintBc]
#[Swag.Inline]
func swap(x, y: *s32)
{
    tmp := dref x
    dref x = dref y
    dref y = tmp
}

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    xxx := 5
    yyy := 10
    swap(&xxx, &yyy)
    @assert(xxx == 10)
    @assert(yyy == 5)
}