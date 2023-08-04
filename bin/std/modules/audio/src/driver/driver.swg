using Core

enum UpdateCommand
{
    Register
    Destroy
    UpdateBuffers
}

// The sound driver
#[Swag.Opaque]
public struct Driver
{
    using native: DriverNative

    updateTh:               Threading.Thread
    wakeupUpdate:           Sync.Event
    mutexVoicesToUpdate:    Sync.Mutex
    voicesToUpdate:         Array'{voice: *Voice, command: UpdateCommand}

    mutexRecycleVoices:     Sync.Mutex
    activeVoices:           Array'(*Voice)
    pendingRecycleVoices:   Array'(*Voice)
    toRecycleVoices:        Array'(*Voice)

    driverIsClosing:        bool
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
        driver.mutexRecycleVoices.lock()
        visit voice: driver.pendingRecycleVoices
            driver.toRecycleVoices.add(voice)
        driver.mutexRecycleVoices.unlock()
        driver.pendingRecycleVoices.clear()

        // Make a copy of the list of voices to update, to reduce contention
        driver.mutexVoicesToUpdate.lock()
        let copyVoices =,move driver.voicesToUpdate
        driver.voicesToUpdate.clear()
        driver.mutexVoicesToUpdate.unlock()

        // Update
        visit cmd: copyVoices
        {
            let voice = cmd.voice
            if th.requestEnd
                break

            switch cmd.command
            {
            case Register:
                voice.idxInList = acast driver.activeVoices.count
                driver.activeVoices.add(voice)

            case Destroy:
                voice.setState(.PendingDestroy)
                catch voice.stopNative()
                voice.destroyNative()

                let idx = voice.idxInList
                driver.activeVoices.removeAt(acast idx)
                if idx != driver.activeVoices.count
                    driver.activeVoices[idx].idxInList = idx
                driver.pendingRecycleVoices.add(voice)

            case UpdateBuffers:
                catch voice.update()

                // If the voice is no more playing, register it for destruction if necessary
                if !voice.isPlaying() and voice.playFlags.has(.DestroyOnStop)
                    voice.destroy()
            }
        }
    }
}

impl Driver
{
    mtd create() throw
    {
        driverIsClosing = false
        createNative()

        wakeupUpdate.init()
        updateTh.init(&updateThreadProc)
        updateTh.start()
    }

    mtd destroy()
    {
        driverIsClosing = true

        // Force update thread to end
        ////////////////
        updateTh.safeForceEnd()
        wakeupUpdate.signal()
        updateTh.wait()
        wakeupUpdate.release()

        // Be sure no update can come during the closing.
        // If there's a Register update, then register the voice to be destroyed, as it's not
        // yet registered in the active list
        ////////////////
        mutexVoicesToUpdate.lock()
        visit cmd: voicesToUpdate
        {
            if cmd.command == .Register
                toRecycleVoices.add(cmd.voice)
        }
        voicesToUpdate.clear()
        mutexVoicesToUpdate.unlock()

        // Destroy lists
        ////////////////
        toRecycleVoices.add(pendingRecycleVoices.toSlice())
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

        // Then destroy native driver
        ////////////////
        destroyNative()
    }

    mtd updateVoice(voice: *Voice, cmd: UpdateCommand)
    {
        mutexVoicesToUpdate.lock()
        defer mutexVoicesToUpdate.unlock()
        if driverIsClosing
            return
        voicesToUpdate.add({voice, cmd})
        wakeupUpdate.signal()
    }

    mtd createVoice()->*Voice
    {
        var back: *Voice
        mutexRecycleVoices.lock()
        if !toRecycleVoices.isEmpty()
            back = toRecycleVoices.popBack()
        mutexRecycleVoices.unlock()
        if back
            back.recycle()
        return back orelse Memory.new'Voice()
    }
}