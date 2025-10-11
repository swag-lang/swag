var X: u32 = undefined

#[Swag.Compiler]
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
