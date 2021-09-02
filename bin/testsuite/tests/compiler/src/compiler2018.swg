var G = 0

struct(T) toto
{
    #run
    {
        /*@print("this is the type: ")
        @print(@typeof(T).name)
        @print("\n")*/
        G += 1
    }
}

#test
{
    var x0: toto's32
    var x1: toto'f64
    if @isbytecode()
        @assert(G == 2)
}