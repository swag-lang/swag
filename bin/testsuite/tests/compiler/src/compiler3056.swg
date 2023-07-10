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
    let to = @typeof(Toto)
    @assert(@countof(to.fields) == 1)
}