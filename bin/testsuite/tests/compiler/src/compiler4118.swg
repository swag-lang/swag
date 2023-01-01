#[Swag.Optim("", false)]
#test
{
    x := 0
    ptr := &x
    @breakpoint()
    @print(x)
}