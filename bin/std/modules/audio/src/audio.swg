using Core

var g_Driver: Driver

// Creates the audio engine.
// Must be called once, before anything else.
public func createEngine() throw
{
    g_Driver.create()
    streamingInit()
}

// Destroy the audio engine.
// Call at the end, when no more used.
public func destroyEngine()
{
    streamingClose()
    g_Driver.destroy()
}

// Get the sound driver unique instance
public func driver()=> &g_Driver
