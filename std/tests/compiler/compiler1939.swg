#module "compiler1939"
#unittest error

struct B
{
    xxx: s32
}

impl B
{
    func toto(using self)
    {
        xxx := 0
    }
}
