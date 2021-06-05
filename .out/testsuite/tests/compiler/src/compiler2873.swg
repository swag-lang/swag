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
    t := @typeof(AA)
    f := t.methods[0].value
    fn := cast(func()) f
    fn()
    @assert(G == 1)
}