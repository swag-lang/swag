using test_foreign

public func toto70()->A70
{
    var r: retval
    r.x = 1
    r.y = 2
    /*r.z = 3*/
    return r
}

#test
{
    call70(&toto70)
}
