enum RGB { R; G; B; }

#[Swag.Using(RGB)]
func toto()
{
    var v = R
}

#test
{
    const to = @typeof(toto)
    @assert(@kindof(to.attributes[0].params[0].value) == RGB)
}