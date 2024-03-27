#test
{
    let contentHeight = 40
    let hasHeader     = true
    var rowHeight:    s32
    if hasHeader
    {
        rowHeight = contentHeight + 50
    }
    else
    {
        rowHeight = contentHeight + 20
    }
    @assert(rowHeight == 90)
}
