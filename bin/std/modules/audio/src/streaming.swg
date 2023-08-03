using Core

private func streamingThreadProc(th: Threading.Thread)
{
    let driver = &g_Driver
    while !th.requestEnd
    {
        driver.wakeupStreaming.wait()

        driver.mutexVoicesToStream.lock()
        let copyVoices = driver.voicesToStream
        driver.voicesToStream.clear()
        driver.mutexVoicesToStream.unlock()

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

impl Driver
{
    mtd streamingInit() throw
    {
        wakeupStreaming.init()
        streamingTh.init(&streamingThreadProc)
        streamingTh.start()
    }

    mtd streamingClose()
    {
        wakeupStreaming.signal()
        streamingTh.safeForceEnd()
        streamingTh.wait()
        wakeupStreaming.release()
        voicesToStream.clear()
    }

    mtd updateVoice(voice: *Voice)
    {
        Sync.scopedLock(&mutexVoicesToStream)
        if voice.pendingDestroy
            return
        voice.pendingStream = true
        voicesToStream.add(voice)
        wakeupStreaming.signal()
    }
}