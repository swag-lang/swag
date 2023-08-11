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

// Set the general output volume
public func setOutputVolume(volume: f32) throw = g_Driver.setOutputVolume(volume)
// Get the general output volume
public func getOutputVolume() => g_Driver.getOutputVolume()
