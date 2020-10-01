//#module "compiler2080"
//#unittest error

struct AA
{

}

impl AA
{
    func opDrop(using self)
    {
        @print("drop\n")
    }
}

func toto()->AA  { return AA{}; }
func titi(x: AA) { }

#[swag.printbc]
#test
{
    titi(toto())
}