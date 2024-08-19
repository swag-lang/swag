#global testerror "Err0270"
enum RGB { R; G; B; }

#[Swag.Using(s32)]
func toto()
{
    var v = R
}

#test
{
    var to = @typeof(toto)
    @assert(@kindof(to.attributes[0].params[0].value) == RGB)
}