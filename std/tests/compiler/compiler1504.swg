#[swag.nobss]
var X: u32

#[swag.compiler]
func set()
{
    X = 6
}

//#[swag.printbc]
#test
{
    #run set()
    @assert(X == 6)
}