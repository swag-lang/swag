using Core

enum UpdateCommand
{
    Register
    Destroy
    UpdateBuffers
}

// The sound driver
struct Driver
{
    using native:             DriverNative

    updateTh:                 Threading.Thread
    wakeupUpdate:             Sync.Event
    mutexVoicesToUpdate:      Sync.Mutex
    voicesToUpdate:           Array'{ voice: *Voice, command: UpdateCommand }

    mutexRecycleVoices:       Sync.Mutex
    activeVoices:             Array'(*Voice)
    pendingRecycleVoices:     Array'(*Voice)
    toRecycleVoices:          Array'(*Voice)

    mutexBuses:               Sync.Mutex
    buses:                    Array'(*Bus)

    driverIsClosing:          bool
}

private func updateThreadProc(th: Threading.Thread)
{
    let driver = &g_Driver
    while !th.requestEnd
    {
        // Wait for an update to be requested
        driver.wakeupUpdate.wait()
        if th.requestEnd do
            break

        // All pending recycles to actual recycle list
        driver.mutexRecycleVoices.lock()
        foreach voice in driver.pendingRecycleVoices do
            driver.toRecycleVoices.add(voice)
        driver.mutexRecycleVoices.unlock()
        driver.pendingRecycleVoices.clear()

        // Make a copy of the list of voices to update, to reduce contention
        driver.mutexVoicesToUpdate.lock()
        let copyVoices = #move driver.voicesToUpdate
        driver.voicesToUpdate.clear()
        driver.mutexVoicesToUpdate.unlock()

        // Update
        foreach cmd in copyVoices
        {
            let voice = cmd.voice
            if th.requestEnd do
                break

            switch cmd.command
            {
            case Register:
                voice.idxInList = cast() driver.activeVoices.count
                driver.activeVoices.add(voice)

            case Destroy:
                voice.setState(.PendingDestroy)
                catch voice.stopNative()
                voice.destroyNative()

                let idx = voice.idxInList
                driver.activeVoices.removeAt(idx)
                if idx != driver.activeVoices.count do
                    driver.activeVoices[idx].idxInList = idx
                driver.pendingRecycleVoices.add(voice)

            case UpdateBuffers:
                // Update buffer of voice, and stop it if nothing has been updated (end reached)
                if catch voice.updateNative() == false
                {
                    voice.closeBuffer()
                    voice.resetState(.Playing)
                }

                // If the voice is no more playing, register it for destruction if necessary
                if !voice.isPlaying() and voice.playFlags.has(.DestroyOnStop) do
                    voice.destroy()
            }
        }
    }
}

impl Driver
{
    mtd create() throw
    {
        .driverIsClosing = false
        .createNative()

        .wakeupUpdate.init()
        .updateTh.init(&updateThreadProc)
        .updateTh.start()
    }

    mtd destroy()
    {
        .driverIsClosing = true

        // Force update thread to end
        ////////////////
        .updateTh.safeForceEnd()
        .wakeupUpdate.signal()
        .updateTh.wait()
        .wakeupUpdate.release()

        // Be sure no update can come during the closing.
        // If there's a Register update, then register the voice to be destroyed, as it's not
        // yet registered in the active list
        ////////////////
        .mutexVoicesToUpdate.lock()
        foreach cmd in .voicesToUpdate
        {
            if cmd.command == .Register do
                .toRecycleVoices.add(cmd.voice)
        }
        .voicesToUpdate.clear()
        .mutexVoicesToUpdate.unlock()

        // Destroy voices lists
        ////////////////
        .toRecycleVoices.add(.pendingRecycleVoices.toSlice())
        .pendingRecycleVoices.clear()

        foreach voice in .toRecycleVoices do
            Memory.delete(voice)
        .toRecycleVoices.clear()

        foreach voice in .activeVoices
        {
            catch voice.stopNative()
            voice.destroyNative()
            Memory.delete(voice)
        }
        .activeVoices.clear()

        // Destroy buses
        ////////////////
        foreach bus in .buses do
            catch bus.destroy()
        .buses.clear()

        // Then destroy native driver
        ////////////////
        .destroyNative()
    }

    mtd updateVoice(voice: *Voice, cmd: UpdateCommand)
    {
        .mutexVoicesToUpdate.lock()
        defer .mutexVoicesToUpdate.unlock()
        if .driverIsClosing do
            return
        .voicesToUpdate.add({voice, cmd})
        .wakeupUpdate.signal()
    }

    mtd createVoice()->*Voice
    {
        var back: *Voice
        .mutexRecycleVoices.lock()
        if !.toRecycleVoices.isEmpty() do
            back = .toRecycleVoices.popBack()
        .mutexRecycleVoices.unlock()
        if back do
            back.recycle()
        return back orelse Memory.new'Voice()
    }
}
