#test
{
    var contentHeight = 40
    var hasHeader = true
    var rowHeight = contentHeight + (hasHeader ? 50 : 20)
    @assert(rowHeight == 90)
}