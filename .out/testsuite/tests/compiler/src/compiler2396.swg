using Swag

#[inline]
func assert(expr: u32)
{
    if expr
    {
    }
}


#[inline]
func isPowerOf2(x: u32)
{
    assert(x)
}

//#[PrintBc]
#test
{
    isPowerOf2(1024)
}