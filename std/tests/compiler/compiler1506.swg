#[swag.compiler]
func set(x: string)
{
    @assert(x == "inside function set")
    //@print(x)
}

//#[swag.printbc]
#test
{    
    x := "inside function set"
    #run 
    {
        set(x)
    }
}