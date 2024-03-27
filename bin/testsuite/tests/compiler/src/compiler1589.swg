struct BABA
{
}

struct(T) Array
{
    buffer: [2] T
}

impl Array
{
    func emplace(using self, x: *T)
    {
        buffer[0] =,nodrop,move dref x
    }
}

#test
{
    var arr:   Array'BABA
    var value: BABA
    arr.emplace(&value)
}
