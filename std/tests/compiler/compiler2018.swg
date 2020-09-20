struct(T) toto
{
    #run
    {
        @print("this is the type: ")
        @print(@typeof(T).name)
        @print("\n")
    }
}

#test
{
    var x0: toto's32
    var x1: toto'f64
}