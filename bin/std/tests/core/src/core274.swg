using Core

typealias GG = s32

#test
{
    var gg: GG = 666
    var sb: StrConv.StringBuilder
    sb.appendFormat("%", gg)
    var str = sb.toString()
    @assert(str == "666")
}