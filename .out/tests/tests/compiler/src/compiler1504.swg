var X: u32 = ?

#[swag.compiler]
func set()
{
    X = 6
}

//#[swag.printbc]
#run
{
    #run set()
    @assert(X == 6)
}