using Core

// The sound driver
#[Swag.Opaque]
public struct Driver
{
    using native: DriverNative

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
        if th.requestEnd
            break

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

            if voice.hasState(.PendingDestroy)
            {
                g_Driver.unregisterVoice(voice)
                Driver.destroyVoice(voice)
                continue
            }

            catch voice.update()
            voice.resetState(.PendingUpdate)

            if !voice.isPlaying() and voice.playFlags.has(.DestroyOnStop)
            {
                voice.destroy()
            }
        }
    }
}

impl Driver
{
    mtd updateInit() throw
    {
        wakeupUpdate.init()
        updateTh.init(&updateThreadProc)
        updateTh.start()
    }

    mtd updateClose()
    {
        updateTh.safeForceEnd()
        wakeupUpdate.signal()
        updateTh.wait()
        wakeupUpdate.release()
        voicesToUpdate.clear()
    }

    mtd registerVoice(voice: *Voice)
    {
        Sync.scopedLock(&mutexAllVoices)
        voice.idxInList = acast allVoices.count
        allVoices.add(voice)
    }

    mtd unregisterVoice(voice: *Voice)
    {
        Sync.scopedLock(&mutexAllVoices)
        let idx = voice.idxInList
        allVoices.removeAt(acast idx)
        if idx != allVoices.count
            allVoices[idx].idxInList = idx
    }

    func destroyVoice(voice: *Voice)
    {
        catch voice.stopNative()
        voice.destroyNative()
        Memory.delete(voice)
    }

    mtd updateVoice(voice: *Voice)
    {
        Sync.scopedLock(&mutexVoicesToUpdate)

        if voice.hasState(.PendingUpdate)
            return
        voice.setState(.PendingUpdate)

        voicesToUpdate.add(voice)
        wakeupUpdate.signal()
    }
}