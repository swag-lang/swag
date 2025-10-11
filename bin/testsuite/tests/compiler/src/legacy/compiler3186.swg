#[Swag.Using(Swag)]
func toto()
{
    let toI = #typeof(toto)
    @assert(@countof(toI.attributes) == 1)
    @assert(@countof(toI.attributes[0].params) == 1)
    @assert(toI.attributes[0].params[0].name == "Swag")
}
