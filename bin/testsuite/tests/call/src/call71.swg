using test_foreign

public func toto71()->A71
{
    var r: retval
    r.x = 1
    r.y = 2
    r.z = 3
    return r
}

#test
{
    call71(&toto71)
}
