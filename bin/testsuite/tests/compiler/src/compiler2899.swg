#global skip
struct AA
{

}

impl AA
{
    func toto(using const me)
    {
    }
}

#test
{
    var v: AA
    var r: const&AA = v
    r.toto()
}