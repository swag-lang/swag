using Core

var g_Driver: Driver

// Creates the audio engine.
// Must be called once, before anything else.
public func createEngine() throw
{
    g_Driver.create()
}

// Destroy the audio engine.
// Call at the end, when no more used.
public func destroyEngine()
{
    g_Driver.destroy()
}

// Get the sound driver unique instance
public func driver()=> &g_Driver

// Convert a DB value to a percent
public func convertDBToPercent(dbVolume: f32)->f32
{
    if dbVolume > -100
        return Math.pow(10.0, dbVolume / 20.0)
    return 0.0
}

// Convert a percent value to DB
public func convertPercentToDB(percentVolume: f32)->f32
{
    if percentVolume > 0.0
        return 20.0 * Math.log10(percentVolume)
    return -100.0
}
