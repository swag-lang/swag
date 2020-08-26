#module "compiler1508"
#unittest error

//#[swag.printbc]
#test
{
    x := 0
    loop 10
    {
        #run
        {
            x = x
        }

        x += 1
    }

    @assert(x == 10)
}