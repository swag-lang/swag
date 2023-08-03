using Core

// The sound driver
#[Swag.Opaque]
public struct Driver
{
    using native: DriverNative

    streamingTh:          Threading.Thread
    wakeupStreaming:      Sync.Event
    mutexVoicesToStream:  Sync.Mutex
    mutexLists:           Sync.Mutex
    voicesToStream:       Array'(*Voice)
    allVoices:            Array'(*Voice)
}

impl Driver
{
    mtd registerVoice(voice: *Voice)
    {
        Sync.scopedLock(&mutexLists)
        allVoices.add(voice)
    }
}

public impl Driver
{
    mtd create() throw
    {
        createNative()
        streamingInit()
    }

    mtd destroy()
    {
        visit v: allVoices
        {
            catch v.stop()
            v.pendingDestroy = true
        }

        streamingClose()
        destroyNative()
    }
}