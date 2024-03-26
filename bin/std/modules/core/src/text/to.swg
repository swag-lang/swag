#global public
#global namespace StrConv

// Convert a size in a displayable format (with Kb, Mb and so on)
func toDisplaySize(value: u64)->String
{
    var size = value
    if size < 1024 do
        return Format.toString("% bytes", size)
    size /= 1024
    if size < 1024 do
        return Format.toString("% Kb", size)
    size /= 1024
    if size < 1024 do
        return Format.toString("% Mb", size)
    size /= 1024
    if size < 1024 do
        return Format.toString("% Gb", size)
    size /= 1024
    return Format.toString("% Tb", size)
}
