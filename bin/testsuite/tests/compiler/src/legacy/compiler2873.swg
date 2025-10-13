#[Swag.ExportType(.Methods)]
struct AA {}

var G = 0
impl AA
{
    func toto()
    {
        G = 1
    }
}

#test
{
    let t  = #typeof(AA)
    let f  = t.methods[0].value
    let fn = cast(func()) f
    fn()
    @assert(G == 1)
}
