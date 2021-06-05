using Core

alias GG = s32

#test
{
    var gg: GG = 666
    var sb: StrConv.StringBuilder
    sb.appendFormat("%", gg)
    str := sb.moveToString()
    @assert(str == "666")
}