#[Swag.Compiler]
func set(x: string)
{
    @assert(x == "inside function set")
}

//#[Swag.PrintBc]
#test
{
    #run
    {
        let x = "inside function set"
        set(x)
    }
}
