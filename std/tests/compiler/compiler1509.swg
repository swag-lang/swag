#[swag.compiler, swag.inline]
func set()
{
    //@print("y := 0")
}

#[swag.printbc]
#test
{    
    x := 0
    loop 10
    {
        #run 
        {
            set()
        }

        x += 1
    }

    @assert(x == 10)
}