#global skip
struct AA
{

}

impl AA
{
    func toto(using const self)
    {
    }
}

#test
{
    var v: AA
    var r: const&AA = v
    r.toto()
}