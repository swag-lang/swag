#module "compiler2146"
#unittest error

struct P
{
}

alias Alias = P
impl Alias
{
    func toto(using self)
    {

    }
}
