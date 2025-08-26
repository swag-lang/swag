#global marked
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
        if th.requestEnd:
            break

        // All pending recycles to actual recycle list
        driver.mutexRecycleVoices.lock()
        foreach voice in driver.pendingRecycleVoices:
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
            if th.requestEnd:
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
                if idx != driver.activeVoices.count:
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
                if !voice.isPlaying() and voice.playFlags.has(.DestroyOnStop):
                    voice.destroy()
            }
        }
    }
}

impl Driver
{
    mtd create() throw
    {
        me.driverIsClosing = false
        me.createNative()

        me.wakeupUpdate.init()
        me.updateTh.init(&updateThreadProc)
        me.updateTh.start()
    }

    mtd destroy()
    {
        me.driverIsClosing = true

        // Force update thread to end
        ////////////////
        me.updateTh.safeForceEnd()
        me.wakeupUpdate.signal()
        me.updateTh.wait()
        me.wakeupUpdate.release()

        // Be sure no update can come during the closing.
        // If there's a Register update, then register the voice to be destroyed, as it's not
        // yet registered in the active list
        ////////////////
        me.mutexVoicesToUpdate.lock()
        foreach cmd in me.voicesToUpdate
        {
            if cmd.command == .Register:
                me.toRecycleVoices.add(cmd.voice)
        }
        me.voicesToUpdate.clear()
        me.mutexVoicesToUpdate.unlock()

        // Destroy voices lists
        ////////////////
        me.toRecycleVoices.add(me.pendingRecycleVoices.toSlice())
        me.pendingRecycleVoices.clear()

        foreach voice in me.toRecycleVoices:
            Memory.delete(voice)
        me.toRecycleVoices.clear()

        foreach voice in me.activeVoices
        {
            catch voice.stopNative()
            voice.destroyNative()
            Memory.delete(voice)
        }
        me.activeVoices.clear()

        // Destroy buses
        ////////////////
        foreach bus in me.buses:
            catch bus.destroy()
        me.buses.clear()

        // Then destroy native driver
        ////////////////
        me.destroyNative()
    }

    mtd updateVoice(voice: *Voice, cmd: UpdateCommand)
    {
        me.mutexVoicesToUpdate.lock()
        defer me.mutexVoicesToUpdate.unlock()
        if me.driverIsClosing:
            return
        me.voicesToUpdate.add({voice, cmd})
        me.wakeupUpdate.signal()
    }

    mtd createVoice()->*Voice
    {
        var back: *Voice
        me.mutexRecycleVoices.lock()
        if !me.toRecycleVoices.isEmpty():
            back = me.toRecycleVoices.popBack()
        me.mutexRecycleVoices.unlock()
        if back:
            back.recycle()
        return back orelse Memory.new'Voice()
    }
}
