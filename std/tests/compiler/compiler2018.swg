struct(T) toto
{
    #print "type is " ~@typeof(T).name
}

#test
{
    var x0: toto's32
    var x1: toto'f64
}