using Core.Random

//#[Swag.PrintBc]
#test
{
    var rng: Rng
    for 100
    {
        let v = rng.nextF64(1.0, 1.1)
        @assert(v > 0.99)
        @assert(v < 1.11)
        //Console.print(v," ")
        //@print(v,"\n")
    }
}
