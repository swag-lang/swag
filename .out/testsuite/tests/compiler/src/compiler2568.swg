func(T) toto()
{
    struct A
    {
        x, y: T
    }

    var v: A
    @assert(@typeof(v.x) == f64)
    #assert @typeof(A.x) == f64
    #assert @sizeof(A) == 2 * @sizeof(f64)
}

#test
{
    toto'f64()
}