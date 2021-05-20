#[Swag.compiler]
func set(x: string)
{
    @assert(x == "inside function set")
    //@print(x)
}

//#[Swag.printbc]
#test
{
    #run
    {
        x := "inside function set"
        set(x)
    }
}