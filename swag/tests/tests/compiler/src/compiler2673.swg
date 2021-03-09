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
    v2 := &v1
    v3 := &v2
    vv := @{v1, v3}
}