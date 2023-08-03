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

// Set the global volume
public func setVolume(volume: f32)
{
    g_Driver.globalVolume = volume
}
