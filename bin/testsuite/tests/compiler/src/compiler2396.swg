using Swag

#[Inline]
func assert(expr: u32)
{
    if expr
    {
    }
}


#[Inline]
func isPowerOf2(x: u32)
{
    assert(x)
}

//#[PrintBc]
#test
{
    isPowerOf2(1024)
}