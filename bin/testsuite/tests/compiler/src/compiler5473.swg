struct A { r, g : s32; }

#[Swag.Strict]
typealias AL = A

//#[Swag.PrintBc]
//#[Swag.Optimize("", false)]
#[Swag.Inline]
func toto()->AL
{
    return {1, 2}
}

#test
{
    let x = toto()
    @assert(@typeof(x) == AL)
}