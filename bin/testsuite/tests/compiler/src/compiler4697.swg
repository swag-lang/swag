//#global testerror
#[Swag.ExportType("methods")]
struct AA
{

}

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
    const t = #typeof(AA)
    let f = t.methods[0].value
    let fn = cast(func()) f
    fn()
    @assert(G == 1)
}