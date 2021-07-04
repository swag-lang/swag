using Core
struct(N: s32) Vector
{
    #ast
    {
        sb := StrConv.StringBuilder{}
        return sb.toString()
    }
}

#test
{
    x := Vector'4{}
}