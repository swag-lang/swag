using Core

// Convert a DB value to a percent
public func convertDBToPercent(dbVolume: f32)->f32
{
    if dbVolume > -100 do
        return Math.pow(10.0, dbVolume / 20.0)
    return 0.0
}

// Convert a percent value to DB
public func convertPercentToDB(percentVolume: f32)->f32
{
    if percentVolume > 0.0 do
        return 20.0 * Math.log10(percentVolume)
    return -100.0
}
