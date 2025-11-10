enum RGB { R, G, B }

#[Swag.Using(RGB)]
func toto()
{
    var v = R
}

#test
{
    const toI = #typeof(toto)
    @assert(@kindof(toI.attributes[0].params[0].value) == RGB)
}
