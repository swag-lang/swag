/*#[swag.compiler, swag.inline]
func set()
{
    @assert(@isbytecode())
}

//#[swag.printbc]
#test
{
    x := 0
    loop 10
    {
        #run
        {
            @assert(@isbytecode())
            set()
        }

        x += 1
    }

    @assert(x == 10)
}*/