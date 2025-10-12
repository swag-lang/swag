#[Swag.Optimize(.All, false)]
#test
{
    let x = -1's8
    let y = @abs(x)
    @assert(y == 1)
}
