using Core

var g_Driver: Driver

// Creates the audio engine.
// Must be called once, before anything else.
public func createEngine() throw
{
    g_Driver.create()
}

// Destroy the audio engine.
// Must be called at the end, when engine is no more used.
public func destroyEngine()
{
    g_Driver.destroy()
}

// Set the general output volume
public func setOutputVolume(volume: f32) throw => g_Driver.setOutputVolume(volume)

// Get the general output volume
public func getOutputVolume()->f32 => g_Driver.getOutputVolume()
