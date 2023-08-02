#global if #os == Swag.TargetOs.Windows
using Core, Win32, XAudio2

#[Swag.Opaque]
public struct DriverXAudio2
{
    xAudio2:          *IXAudio2
    masteringVoice:   *IXAudio2MasteringVoice
    hasInitCOM:       bool
    channelMask:      DWORD
    x3DInstance:      X3DAUDIO_HANDLE
}

public typealias VoiceHandle = *IXAudio2SourceVoice

public impl DriverXAudio2
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

    mtd createVoice(file: *SoundFile)->Voice throw
    {
        let infos = &file.infos

        var wfx: WAVEFORMATEXTENSIBLE
        wfx.format.nChannels            = cast(u16) infos.channelCount
        wfx.format.nSamplesPerSec       = cast(u16) infos.frequency
        wfx.format.wBitsPerSample       = cast(u16) infos.bitsPerSample
        wfx.format.nBlockAlign          = cast(u16) (wfx.format.nChannels * (wfx.format.wBitsPerSample / 8))
        wfx.format.nAvgBytesPerSec      = wfx.format.nBlockAlign * wfx.format.nSamplesPerSec
        wfx.format.wFormatTag           = FORMAT_EXTENSIBLE
        wfx.format.cbSize               = @sizeof(WAVEFORMATEXTENSIBLE) - @sizeof(WAVEFORMATEX)
        wfx.samples.wValidBitsPerSample = wfx.format.wBitsPerSample
        wfx.dwChannelMask               = infos.channelMask

        switch (infos.format)
        {
        case FloatIEEE: wfx.subFormat = GUID_FLOAT_IEEE
        case Pcm:       wfx.subFormat = GUID_PCM
        }

        var result: retval
        let hr = xAudio2.vtbl.createSourceVoice(xAudio2, &result.handle, acast &wfx.format, 0/*XAUDIO2_VOICE_USEFILTER*/, XAUDIO2_DEFAULT_FREQ_RATIO, null, null, null)
        if hr != S_OK
            throw "cannot create source voice"

        result.file = file
        return result
    }

    mtd playVoice(voice: *Voice) throw
    {
        let dataSize = voice.file.infos.dataSize
        let firstSample = 0
        let looping = false

        var buffer: XAUDIO2_BUFFER
        buffer.pAudioData     = voice.file.datas.buffer
        buffer.flags          = dataSize == voice.file.infos.dataSize ? XAUDIO2_END_OF_STREAM : 0
        buffer.audioBytes     = acast dataSize
        buffer.playBegin      = firstSample
        buffer.pContext       = null
        if looping and dataSize == voice.file.infos.dataSize
            buffer.loopCount = XAUDIO2_LOOP_INFINITE

        //m_FirstPlayed = true;
        var hr = voice.handle.vtbl.submitSourceBuffer(voice.handle, &buffer, null)
        if hr != S_OK
            throw "cannot play"

        hr = voice.handle.vtbl.start(voice.handle, 0, 0)
        if hr != S_OK
            throw "cannot play"

        Threading.Thread.sleep(2's)
    }
}