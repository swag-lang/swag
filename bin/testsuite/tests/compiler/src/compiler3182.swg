enum RGB { R; G; B; }

#[Swag.Using(RGB)]
func toto()
{
    v := R
}

#test
{
    to := @typeof(toto)
    @assert(@kindof(to.attributes[0].params[0].value) == RGB)
}