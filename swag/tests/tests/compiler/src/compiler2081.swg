//
//#global testerror

struct AA
{

}

var G = 0

impl AA
{
    func opDrop(using self)
    {
        G += 1
    }
}

func toto()->AA  { return AA{}; }
func titi(x: AA) { }

//#[swag.printbc]
#test
{
    {
        titi(toto())
    }

    @assert(G == 1)
}