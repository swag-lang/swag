#[Swag.ExportType("methods")]
struct Toto
{
    x: s32
}

impl Toto
{
    func titi() {}
}

#test
{
    var to = @typeof(Toto)
    @assert(@countof(to.fields) == 1)
}