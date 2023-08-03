using Core

var g_Driver:           Driver
var g_Streaming:        Threading.Thread
var g_WakeupStreaming:  Sync.Event

private func streamingEntry(th: Threading.Thread)
{
    while !th.requestEnd
    {
        g_WakeupStreaming.wait()
    }
}

// Creates the audio engine.
// Must be called once, before anything else.
public func createEngine() throw
{
    g_Driver.create()

    g_WakeupStreaming.init()
    g_Streaming.init(&streamingEntry)
    g_Streaming.start()
}

// Destroy the audio engine.
// Call at the end, when no more used.
public func destroyEngine()
{
    g_WakeupStreaming.signal()
    g_Streaming.safeForceEnd()
    g_Streaming.wait()
    g_WakeupStreaming.release()

    g_Driver.destroy()
}

// Get the sound driver unique instance
public func driver()=> &g_Driver
