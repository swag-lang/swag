using Core

// The sound driver
#[Swag.Opaque]
public struct Driver
{
    using native: DriverNative

    globalVolume:           f32 = 1
    updateTh:               Threading.Thread
    wakeupUpdate:           Sync.Event
    mutexVoicesToUpdate:    Sync.Mutex
    voicesToUpdate:         Array'(*Voice)

    mutexAllVoices:         Sync.Mutex
    allVoices:              Array'(*Voice)
}

public impl Driver
{
    mtd create() throw
    {
        createNative()
        updateInit()
    }

    mtd destroy()
    {
        updateClose()

        visit voice: allVoices
            destroyVoice(voice)
        allVoices.clear()

        destroyNative()
    }
}


private func updateThreadProc(th: Threading.Thread)
{
    let driver = &g_Driver
    while !th.requestEnd
    {
        // Wait for an update to be requested
        driver.wakeupUpdate.wait()

        // Make a copy of the list of voices to update, to reduce contention
        driver.mutexVoicesToUpdate.lock()
        let copyVoices = driver.voicesToUpdate
        driver.voicesToUpdate.clear()
        driver.mutexVoicesToUpdate.unlock()

        // Update
        visit voice: copyVoices
        {
            if th.requestEnd
                break

            catch voice.update()

            if Atomic.get(&voice.pendingDestroy)
                g_Driver.destroyUnregisterVoice(voice)

            Atomic.and(&voice.pendingUpdate, 0)
        }
    }
}

impl Driver
{
    mtd registerVoice(voice: *Voice)
    {
        Sync.scopedLock(&mutexAllVoices)
        voice.idxInList = acast allVoices.count
        allVoices.add(voice)
    }

    mtd updateInit() throw
    {
        wakeupUpdate.init()
        updateTh.init(&updateThreadProc)
        updateTh.start()
    }

    mtd updateClose()
    {
        wakeupUpdate.signal()
        updateTh.safeForceEnd()
        updateTh.wait()
        wakeupUpdate.release()
        voicesToUpdate.clear()
    }

    func destroyVoice(voice: *Voice)
    {
        catch voice.stop()
        voice.destroyNative()
        Memory.delete(voice)
    }

    mtd destroyUnregisterVoice(voice: *Voice)
    {
        // Stop and destroy voice
        let idx = voice.idxInList
        destroyVoice(voice)

        // Remove from voice list
        Sync.scopedLock(&mutexAllVoices)
        allVoices.removeAt(acast idx)
        if idx != allVoices.count
            allVoices[idx].idxInList = idx
    }

    mtd updateVoice(voice: *Voice)
    {
        Sync.scopedLock(&mutexVoicesToUpdate)

        if Atomic.get(&voice.pendingUpdate)
            return
        Atomic.or(&voice.pendingUpdate, 1)

        voicesToUpdate.add(voice)
        wakeupUpdate.signal()
    }
}