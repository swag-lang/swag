using Core

alias GG = s32

#test
{
    let gg: GG = 666
    var sb: StrConv.StringBuilder
    sb.appendFormat("%", gg)
    let str = sb.toString()
    @assert(str == "666")
}
