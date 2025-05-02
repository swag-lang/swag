struct A
{

}

var G : s32 = 0

impl A
{
    func opDrop(using self)
    {
        G += 1
    }
}

#[Swag.Macro]
func toto(stmt: #code void)
{
    if true:
        return
    for 4
    {
        #macro
        {
            #inject #up stmt
        }
    }
}

//#[Swag.PrintBc]
#test
{
    G = 0
    var x: A
    toto()
    {

    }

    @assert(G == 0)
}