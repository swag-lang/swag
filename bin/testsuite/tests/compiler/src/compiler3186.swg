#[Swag.Using(Swag)]
func toto()
{
    var to = @typeof(toto)
    @assert(@countof(to.attributes) == 1)
    @assert(@countof(to.attributes[0].params) == 1)
    @assert(to.attributes[0].params[0].name == "Swag")
}