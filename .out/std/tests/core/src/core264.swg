using Core.Random

//#[Swag.printbc]
#test
{
	var rng: Rng
    loop 100
    {
        v := rng.nextF64(1.0, 1.1)
        @assert(v > 0.99)
        @assert(v < 1.11)
        //Console.print(v," ")
        //@print(v,"\n")
    }
}