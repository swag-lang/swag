struct AA {}

var G = 0

impl AA
{
    func titi(me)
    {
        G += 1
    }

    func toto(me)
    {
        var x: func(*AA)
        x = &titi
        x(me)

        var y: #decltype(&titi)
        y = &titi
        y(me)
    }
}

#test
{
    var x: AA
    x.toto()
    @assert(G == 2)
}
