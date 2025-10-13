#[Swag.ExportType(.Methods)]
struct Toto
{
    x:   s32
}

impl Toto
{
    func titi() {}
}

#test
{
    let toI = #typeof(Toto)
    @assert(@countof(toI.fields) == 1)
}
