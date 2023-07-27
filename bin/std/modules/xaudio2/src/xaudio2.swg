#global export
#global if #os == Swag.TargetOs.Windows
#foreignlib "xaudio2"
using Swag, Win32

alias UINT32  = u32
alias UINT64  = u64
alias FLOAT32 = f32

const X3DAUDIO_HANDLE_BYTESIZE = 20
alias X3DAUDIO_HANDLE = [X3DAUDIO_HANDLE_BYTESIZE] BYTE

alias XAUDIO2_PROCESSOR = UINT32
const Processor1  = 0x00000001
const Processor2  = 0x00000002
const Processor3  = 0x00000004
const Processor4  = 0x00000008
const Processor5  = 0x00000010
const Processor6  = 0x00000020
const Processor7  = 0x00000040
const Processor8  = 0x00000080
const Processor9  = 0x00000100
const Processor10 = 0x00000200
const Processor11 = 0x00000400
const Processor12 = 0x00000800
const Processor13 = 0x00001000
const Processor14 = 0x00002000
const Processor15 = 0x00004000
const Processor16 = 0x00008000
const Processor17 = 0x00010000
const Processor18 = 0x00020000
const Processor19 = 0x00040000
const Processor20 = 0x00080000
const Processor21 = 0x00100000
const Processor22 = 0x00200000
const Processor23 = 0x00400000
const Processor24 = 0x00800000
const Processor25 = 0x01000000
const Processor26 = 0x02000000
const Processor27 = 0x04000000
const Processor28 = 0x08000000
const Processor29 = 0x10000000
const Processor30 = 0x20000000
const Processor31 = 0x40000000
const Processor32 = 0x80000000
const XAUDIO2_ANY_PROCESSOR = 0xffffffff

const X3DAUDIO_SPEED_OF_SOUND = 343.5

const XAUDIO2_DEFAULT_CHANNELS = 0
const XAUDIO2_DEFAULT_SAMPLERATE = 0

struct XAUDIO2_EFFECT_CHAIN
{
}

struct XAUDIO2_PERFORMANCE_DATA
{
    AudioCyclesSinceLastQuery:    UINT64
    TotalCyclesSinceLastQuery:    UINT64
    MinimumCyclesPerQuantum:      UINT32
    MaximumCyclesPerQuantum:      UINT32
    MemoryUsageInBytes:           UINT32
    CurrentLatencyInSamples:      UINT32
    GlitchesSinceEngineStarted:   UINT32
    ActiveSourceVoiceCount:       UINT32
    TotalSourceVoiceCount:        UINT32
    ActiveSubmixVoiceCount:       UINT32
    ActiveResamplerCount:         UINT32
    ActiveMatrixMixCount:         UINT32
    ActiveXmaSourceVoices:        UINT32
    ActiveXmaStreams:             UINT32
}

struct XAUDIO2_DEBUG_CONFIGURATION
{
    TraceMask:        UINT32
    BreakMask:        UINT32
    LogThreadID:      BOOL
    LogFileline:      BOOL
    LogFunctionName:  BOOL
    LogTiming:        BOOL
}

enum AUDIO_STREAM_CATEGORY
{
    AudioCategory_Other = 0,
    AudioCategory_ForegroundOnlyMedia = 1,
    AudioCategory_BackgroundCapableMedia = 2,
    AudioCategory_Communications = 3,
    AudioCategory_Alerts = 4,
    AudioCategory_SoundEffects = 5,
    AudioCategory_GameEffects = 6,
    AudioCategory_GameMedia = 7,
    AudioCategory_GameChat = 8,
    AudioCategory_Speech = 9,
    AudioCategory_Movie = 10,
    AudioCategory_Media = 11,
}

struct XAUDIO2_VOICE_DETAILS
{
    CreationFlags:      UINT32
    ActiveFlags:        UINT32
    InputChannels:      UINT32
    InputSampleRate:    UINT32
}

struct XAUDIO2_SEND_DESCRIPTOR
{
    Flags:          UINT32
    pOutputVoice:   *IXAudio2Voice;
}

struct XAUDIO2_VOICE_SENDS
{
    SendCount:  UINT32
    pSends:     *XAUDIO2_SEND_DESCRIPTOR
}

struct IXAudio2                 { vtbl: *IXAudio2Itf; }
struct IXAudio2Voice            { vtbl: *IXAudio2VoiceItf; }
struct IXAudio2MasteringVoice   { vtbl: *IXAudio2MasteringVoiceItf; }
struct IXAudio2EngineCallback   { }

struct IXAudio2Itf
{
    QueryInterface:         *void
    AddRef:                 func()->ULONG;
    Release:                func(me: *IXAudio2)->ULONG
    RegisterForCallbacks:   func(pCallback: *IXAudio2EngineCallback);
    UnregisterForCallbacks: func(pCallback: *IXAudio2EngineCallback);
    CreateSourceVoice:      *void
    CreateSubmixVoice:      *void
    CreateMasteringVoice:   func(me: *IXAudio2, voice: **IXAudio2MasteringVoice, inputChannels: UINT32 = XAUDIO2_DEFAULT_CHANNELS, inputSampleRate: UINT32 = XAUDIO2_DEFAULT_SAMPLERATE, flags: UINT32 = 0, szDeviceId: LPCWSTR = null, effectChain: *XAUDIO2_EFFECT_CHAIN = null, streamCategory = AUDIO_STREAM_CATEGORY.AudioCategory_GameEffects)->HRESULT
    StartEngine:            func()->HRESULT;
    StopEngine:             func()->HRESULT;
    CommitChanges:          func(operationSet: UINT32)->HRESULT;
    GetPerformanceData:     func(pPerfData: *XAUDIO2_PERFORMANCE_DATA);
    SetDebugConfiguration:  func(pDebugConfiguration: const *XAUDIO2_DEBUG_CONFIGURATION, pReserved: *void);
}

struct IXAudio2VoiceItf
{
    GetVoiceDetails:            func(me: *IXAudio2Voice, pVoiceDetails: *XAUDIO2_VOICE_DETAILS)
    SetOutputVoices:            func(me: *IXAudio2Voice, pSendList: const *XAUDIO2_VOICE_SENDS)
    SetEffectChain:             func(me: *IXAudio2Voice)
    EnableEffect:               func(me: *IXAudio2Voice)
    DisableEffect:              func(me: *IXAudio2Voice)
    GetEffectState:             func(me: *IXAudio2Voice)
    SetEffectParameters:        func(me: *IXAudio2Voice)
    GetEffectParameters:        func(me: *IXAudio2Voice)
    SetFilterParameters:        func(me: *IXAudio2Voice)
    GetFilterParameters:        func(me: *IXAudio2Voice)
    SetOutputFilterParameters:  func(me: *IXAudio2Voice)
    GetOutputFilterParameters:  func(me: *IXAudio2Voice)
    SetVolume:                  func(me: *IXAudio2Voice)
    GetVolume:                  func(me: *IXAudio2Voice)
    SetChannelVolumes:          func(me: *IXAudio2Voice)
    GetChannelVolumes:          func(me: *IXAudio2Voice)
    SetOutputMatrix:            func(me: *IXAudio2Voice)
    GetOutputMatrix:            func(me: *IXAudio2Voice)
    DestroyVoice:               func(me: *IXAudio2Voice)
}

struct IXAudio2MasteringVoiceItf
{
    using voice: IXAudio2VoiceItf
    GetChannelMask: func(me: *IXAudio2MasteringVoice, pChannelmask: *DWORD)->HRESULT;
}

var g_XAudio2:          *IXAudio2
var g_MasteringVoice:   *IXAudio2MasteringVoice
var g_InitCOM:          bool
var g_ChannelMask:      DWORD
var g_X3DInstance:      X3DAUDIO_HANDLE

#[Swag.Foreign("xaudio2_9")]
{
    func XAudio2Create(ppXAudio2: **IXAudio2, flags: UINT32, processor: XAUDIO2_PROCESSOR = XAUDIO2_ANY_PROCESSOR)->HRESULT;
    func X3DAudioInitialize(speakerChannelMask: UINT32, speedOfSound: FLOAT32, instance: X3DAUDIO_HANDLE)->HRESULT;
}

// Create the xaudio2 engine
func create(initCOM: bool = true) throw
{
    //Debug.assert(!g_XAudio2)

    var result: HRESULT
    defer(err) destroy()

    if initCOM
    {
        result = CoInitializeEx(null, 0)
        if result != S_OK
            throw "failed to initialize COM"
        g_InitCOM = initCOM
    }

    result = XAudio2Create(&g_XAudio2, 0)
    if result != S_OK
        throw "cannot create xaudio2 device"

    result = g_XAudio2.vtbl.CreateMasteringVoice(g_XAudio2, &g_MasteringVoice)
    if result != S_OK
        throw "cannot create xaudio2 mastering voice"

    result = g_MasteringVoice.vtbl.GetChannelMask(g_MasteringVoice, &g_ChannelMask)
    if result != S_OK
        throw "cannot get xaudio2 channel mask"

    result = X3DAudioInitialize(g_ChannelMask, X3DAUDIO_SPEED_OF_SOUND, g_X3DInstance)
    if result != S_OK
        throw "cannot initialize x3D audio"
}

// Destroys the xaudio2 engine
func destroy()
{
    if g_MasteringVoice
    {
        g_MasteringVoice.vtbl.DestroyVoice(acast g_MasteringVoice)
        g_MasteringVoice = null
    }

    if g_XAudio2
    {
        discard g_XAudio2.vtbl.Release(g_XAudio2)
        g_XAudio2 = null
    }

    if g_InitCOM
    {
        CoUninitialize()
        g_InitCOM = false
    }
}
