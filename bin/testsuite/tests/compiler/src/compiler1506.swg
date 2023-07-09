#[Swag.Compiler]
func set(x: string)
{
    @assert(x == "inside function set")
    //@print(x)
}

//#[Swag.PrintBc]
#test
{
    #run
    {
        var x = "inside function set"
        set(x)
    }
}