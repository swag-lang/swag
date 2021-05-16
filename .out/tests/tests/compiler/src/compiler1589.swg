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
        buffer[0] =,nodrop,move x[0]
    }
}

#test
{
    var arr: Array'BABA
    var value: BABA
    arr.emplace(&value)
}