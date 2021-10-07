#global testerror
enum RGB { R; G; B; }

#[Swag.Using(s32)]
func toto()
{
    v := R
}

#test
{
    to := @typeof(toto)
    @assert(@kindof(to.attributes[0].params[0].value) == RGB)
}