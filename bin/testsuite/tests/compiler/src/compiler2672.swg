#global skip
struct AA
{
    x: f64 = 1
    y: f64 = 2
    z: f64 = 3
}

#test
{
    var v: AA
    var v1: const&AA = v
    var v2 = &v1
    var vv = {v1, v2}
}