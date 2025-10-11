#global public
#global namespace StrConv

// Converts a size in bytes into a human-readable string with units (bytes, Kb, Mb, Gb, Tb)
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

// Converts a bit rate value into a readable format with Kbps, Mbps, or Gbps
func toDisplayBitrate(value: u64)->String
{
    var rate = value
    if rate < 1000 do
        return Format.toString("% bps", rate)
    rate /= 1000
    if rate < 1000 do
        return Format.toString("% Kbps", rate)
    rate /= 1000
    if rate < 1000 do
        return Format.toString("% Mbps", rate)
    rate /= 1000
    return Format.toString("% Gbps", rate)
}

// Converts a duration in milliseconds into a string formatted as "Xd Yh Zm Ws"
func toDisplayDuration(ms: u64)->String
{
    let seconds = ms / 1000
    let days    = seconds / 86400
    let hours   = (seconds % 86400) / 3600
    let minutes = (seconds % 3600) / 60
    let secs    = seconds % 60

    if days > 0 do
        return Format.toString("%d %h %m %s", days, hours, minutes, secs)
    if hours > 0 do
        return Format.toString("%h %m %s", hours, minutes, secs)
    if minutes > 0 do
        return Format.toString("%m %s", minutes, secs)
    return Format.toString("%s", secs)
}
