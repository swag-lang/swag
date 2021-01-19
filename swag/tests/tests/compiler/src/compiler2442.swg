struct A
{
    x, y: u64
}

func toto()->{A, string}
{
    var result: retval
    return result
}

#[swag.printbc]
#test
{
    (v, err) := toto()
}