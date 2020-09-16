#[swag.inline]
func swap(x, y: *s32)
{
    deref x = deref y
}

#[swag.printbc]
#[swag.optimizebc(0)]
#test
{
    x := 5
    //swap(&x, &x)
}