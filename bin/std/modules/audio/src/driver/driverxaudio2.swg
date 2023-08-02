#global if #os == Swag.TargetOs.Windows
using Core, Win32, XAudio2

#[Swag.Opaque]
public struct DriverNative
{
    xAudio2:          *IXAudio2
    masteringVoice:   *IXAudio2MasteringVoice
    hasInitCOM:       bool
    channelMask:      DWORD
    x3DInstance:      X3DAUDIO_HANDLE
}

public typealias VoiceHandle = *IXAudio2SourceVoice

public impl DriverNative
{
    // Create the xaudio2 engine
    mtd create(initCOM: bool = true) throw
    {
        Debug.assert(!xAudio2)

        var result: HRESULT
        defer(err) destroy()

        if initCOM
        {
            result = CoInitializeEx(null, 0)
            if result != S_OK
                throw "failed to initialize COM"
            hasInitCOM = initCOM
        }

        result = XAudio2Create(&xAudio2, 0)
        if result != S_OK
            throw "cannot create xaudio2 device"

        result = xAudio2.vtbl.createMasteringVoice(xAudio2, &masteringVoice)
        if result != S_OK
            throw "cannot create xaudio2 mastering voice"

        var debug: XAUDIO2_DEBUG_CONFIGURATION
        debug.traceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS
        debug.breakMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS
        xAudio2.vtbl.setDebugConfiguration(xAudio2, &debug, null)

        result = masteringVoice.vtbl.getChannelMask(masteringVoice, &channelMask)
        if result != S_OK
            throw "cannot get xaudio2 channel mask"

        result = X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, x3DInstance)
        if result != S_OK
            throw "cannot initialize X3D audio"
    }

    // Destroys the xaudio2 engine
    mtd destroy()
    {
        if masteringVoice
        {
            masteringVoice.vtbl.destroyVoice(acast masteringVoice)
            masteringVoice = null
        }

        if xAudio2
        {
            discard xAudio2.vtbl.release(xAudio2)
            xAudio2 = null
        }

        if hasInitCOM
        {
            CoUninitialize()
            hasInitCOM = false
        }
    }
}

public impl Voice
{
    // Creates a new voice based on the given file.
    func create(file: *SoundFile)->Voice throw
    {
        var wfx: WAVEFORMATEXTENSIBLE
        wfx.format.nChannels            = cast(u16) file.channelCount
        wfx.format.nSamplesPerSec       = cast(u16) file.frequency
        wfx.format.wBitsPerSample       = cast(u16) file.bitsPerSample
        wfx.format.nBlockAlign          = cast(u16) (wfx.format.nChannels * (wfx.format.wBitsPerSample / 8))
        wfx.format.nAvgBytesPerSec      = wfx.format.nBlockAlign * wfx.format.nSamplesPerSec
        wfx.format.wFormatTag           = FORMAT_EXTENSIBLE
        wfx.format.cbSize               = @sizeof(WAVEFORMATEXTENSIBLE) - @sizeof(WAVEFORMATEX)
        wfx.samples.wValidBitsPerSample = wfx.format.wBitsPerSample
        wfx.dwChannelMask               = file.channelMask

        switch (file.format)
        {
        case FloatIEEE: wfx.subFormat = GUID_FLOAT_IEEE
        case Pcm:       wfx.subFormat = GUID_PCM
        }

        var result: retval
        let hr = g_Driver.xAudio2.vtbl.createSourceVoice(g_Driver.xAudio2, &result.handle, acast &wfx.format, XAUDIO2_VOICE_USEFILTER, XAUDIO2_DEFAULT_FREQ_RATIO, null, null, null)
        if hr != S_OK
            throw "cannot create source voice"

        result.file = file
        return result
    }

    // Destroy the voice
    mtd destroy()
    {
        if !handle
            return

        handle.vtbl.destroyVoice(acast handle)
        handle = null
    }

    // Play the sound
    mtd play(looping = false, firstSample: u32 = 0, batchID: u32 = 0) throw
    {
        if !handle
            return

        let dataSize = file.datas.count
        if dataSize
        {
            var buffer: XAUDIO2_BUFFER
            buffer.pAudioData     = file.datas.buffer
            buffer.flags          = dataSize == file.dataSize ? XAUDIO2_END_OF_STREAM : 0
            buffer.audioBytes     = acast dataSize
            buffer.playBegin      = firstSample
            buffer.pContext       = null
            if looping and dataSize == file.dataSize
                buffer.loopCount = XAUDIO2_LOOP_INFINITE

            let hr = handle.vtbl.submitSourceBuffer(handle, &buffer, null)
            if hr != S_OK
                throw "cannot play"
        }

        let hr = handle.vtbl.start(handle, 0, batchID)
        if hr != S_OK
            throw "cannot play"
    }

    // Stop the playing voice
    mtd stop(batchID: u32 = 0) throw
    {
        if !handle
            return
        let hr = handle.vtbl.stop(handle, 0, batchID)
        if hr != S_OK
            throw "error"
    }

    // Set the playing voice volume between [0..1]
    mtd setVolume(volume: f32, batchID: u32 = 0) throw
    {
        if !handle
            return
        let hr = handle.vtbl.setVolume(acast handle, Math.clamp(volume, 0, 1), batchID)
        if hr != S_OK
            throw "error"
    }

    // Set the playing voice volume
    mtd setVolumeDB(volumeDB: f32, batchID: u32 = 0) throw
    {
        if !handle
            return
        let hr = handle.vtbl.setVolume(acast handle, convertDBToPercent(volumeDB), batchID)
        if hr != S_OK
            throw "error"
    }

    // Returns true if the voice is currently playing
    mtd isPlaying()->bool
    {
        if !handle
            return false
        var voiceState: XAUDIO2_VOICE_STATE
        handle.vtbl.getState(handle, &voiceState, 0)
        return voiceState.buffersQueued != 0
    }
}