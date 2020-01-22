struct S
{

}

var G = 0

impl S
{
    #[swag.macro, swag.noreturn]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        #if ptr
            G += 1
        #else
            G += 2
    }
}

//#[swag.printbc]
#test
{
    G = 0

    var x: S
    visit x {}
    @assert(G == 2)
    visit x {}    
    @assert(G == 4)
    visit *xxx: x {} 
    @assert(G == 5)
}