#[Swag.Optimize(.All, false)]
#test
{
    let x = ["45cm"]
    let y = x[0][2 to ]
    @assert(y == "cm")
}
