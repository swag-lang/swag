/*#[Swag.compiler]
func set(x: string)
{
    @assert(x == "inside function set")
    //@print(x)
}

//#[Swag.PrintBc]
#test
{
    x := 0
    loop 10
    {
        #run
        {
            //@print("toto")
            //x = x
            y := 6
        }

        x += 1
    }

    @assert(x == 10)
}*/