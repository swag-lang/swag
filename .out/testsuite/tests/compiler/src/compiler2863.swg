struct AA
{

}

var G = 0

impl AA
{
    func titi(using self)
    {
        G += 1
    }

    func toto(using self)
    {
        var x: func(*AA)
        x = &titi
        x(self)

        var y: @typeof(&titi)
        y = &titi
        y(self)
    }
}

#test
{
    var x: AA
    x.toto()
    @assert(G == 2)
}