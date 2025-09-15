using Core
struct(N: s32) Vector
{
    #ast
    {
        let sb = StrConv.StringBuilder{}
        return sb.toString()
    }
}

#test
{
    var x = Vector'4{}
}
