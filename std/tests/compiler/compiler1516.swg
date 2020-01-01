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

#[swag.macro]
func toto(stmt: code)
{
    if true
        return
    loop 4
    {
        #macro
        {
            #mixin `stmt
        }
    }
}

//#[swag.printbc]
#test
{
    G = 0
    var x: A
    toto()
    {

    }

    @assert(G == 0)
}