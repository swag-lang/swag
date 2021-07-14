var X: u32 = ?

#[Swag.compiler]
func set()
{
    X = 6
}

//#[Swag.PrintBc]
#run
{
    #run set()
    @assert(X == 6)
}