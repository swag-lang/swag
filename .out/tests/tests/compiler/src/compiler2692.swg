struct(T) Array
{
    buffer: *T
    #assert @sizeof(T) > 0
}

struct Contribution
{
    weights: Array'f64
}

struct LineContrib
{
    row: Array'Contribution
}

#test
{
    var x: LineContrib
}