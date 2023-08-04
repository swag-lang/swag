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

    mutexActiveVoices:      Sync.Mutex
    activeVoices:           Array'(*Voice)
    pendingRecycleVoices:   Array'(*Voice)
    toRecycleVoices:        Array'(*Voice)
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

        // All pending recycles to actual recycle list
        visit voice: driver.pendingRecycleVoices
            driver.toRecycleVoices.add(voice)
        driver.pendingRecycleVoices.clear()

        // Make a copy of the list of voices to update, to reduce contention
        driver.mutexVoicesToUpdate.lock()
        let copyVoices =,move driver.voicesToUpdate
        driver.voicesToUpdate.clear()
        driver.mutexVoicesToUpdate.unlock()

        // Update
        visit voice: copyVoices
        {
            if th.requestEnd
                break

            if voice.hasState(.PendingDestroy)
            {
                driver.unregisterVoice(voice)
                catch voice.stopNative()
                voice.destroyNative()
                driver.pendingRecycleVoices.add(voice)
                continue
            }

            catch voice.update()
            voice.resetState(.PendingUpdate)

            // If the voice is no more playing, register it for destruction if necessary
            if !voice.isPlaying() and voice.playFlags.has(.DestroyOnStop)
                voice.destroy()
        }
    }
}

impl Driver
{
    mtd create() throw
    {
        createNative()

        wakeupUpdate.init()
        updateTh.init(&updateThreadProc)
        updateTh.start()
    }

    mtd destroy()
    {
        updateTh.safeForceEnd()
        wakeupUpdate.signal()
        updateTh.wait()
        wakeupUpdate.release()
        voicesToUpdate.clear()

        visit voice: pendingRecycleVoices
            Memory.delete(voice)
        pendingRecycleVoices.clear()

        visit voice: toRecycleVoices
            Memory.delete(voice)
        toRecycleVoices.clear()

        visit voice: activeVoices
        {
            catch voice.stopNative()
            voice.destroyNative()
            Memory.delete(voice)
        }
        activeVoices.clear()

        destroyNative()
    }

    mtd registerVoice(voice: *Voice)
    {
        Sync.scopedLock(&mutexActiveVoices)
        voice.idxInList = acast activeVoices.count
        activeVoices.add(voice)
    }

    mtd unregisterVoice(voice: *Voice)
    {
        Sync.scopedLock(&mutexActiveVoices)
        let idx = voice.idxInList
        activeVoices.removeAt(acast idx)
        if idx != activeVoices.count
            activeVoices[idx].idxInList = idx
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