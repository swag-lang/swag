//#[Swag.PrintBc]
#[Swag.Inline]
func swap(x, y: *s32)
{
    var tmp = dref x
    dref x = dref y
    dref y = tmp
}

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    var xxx = 5
    var yyy = 10
    swap(&xxx, &yyy)
    @assert(xxx == 10)
    @assert(yyy == 5)
}