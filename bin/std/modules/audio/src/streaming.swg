using Core

var g_Streaming:            Threading.Thread
var g_WakeupStreaming:      Sync.Event
var g_VoicesToStreamMutex:  Sync.Mutex
var g_VoicesToStream:       Array'(*Voice)

func streamingInit() throw
{
    g_WakeupStreaming.init()
    g_Streaming.init(&streamingThreadProc)
    g_Streaming.start()
}

func streamingClose()
{
    g_WakeupStreaming.signal()
    g_Streaming.safeForceEnd()
    g_Streaming.wait()
    g_WakeupStreaming.release()
    g_VoicesToStream.clear()
}

func streamingThreadProc(th: Threading.Thread)
{
    while !th.requestEnd
    {
        g_WakeupStreaming.wait()

        g_VoicesToStreamMutex.lock()
        let copyVoices = g_VoicesToStream
        g_VoicesToStream.clear()
        g_VoicesToStreamMutex.unlock()

        visit voice: copyVoices
        {
            if th.requestEnd
                break
            if voice.pendingDestroy
                continue
            catch voice.stream()
            voice.pendingStream = false
        }
    }
}

func addStreamingVoice(voice: *Voice)
{
    Sync.scopedLock(&g_VoicesToStreamMutex)
    if voice.pendingDestroy
        return
    voice.pendingStream = true
    g_VoicesToStream.add(voice)
    g_WakeupStreaming.signal()
}
