#[Swag.Optimize("", false)]
#test
{
    let x = ["45cm"]
    let y = x[0][2..]
    @assert(y == "cm")
}