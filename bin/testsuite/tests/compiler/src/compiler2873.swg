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
    var t = @typeof(AA)
    var f = t.methods[0].value
    var fn = cast(func()) f
    fn()
    @assert(G == 1)
}