/*
'Std.Audio' is a module to decode and play sound files.
Under windows, it is based on the 'xaudio2' library.
# How to play a sound
First, you have to initialize the audio engine by calling [Audio.createEngine].
```
Audio.createEngine()
defer Audio.destroyEngine() // Don't forget to destroy the engine when you are done
```
You then have to load a sound file.
```
let soundFile = Audio.SoundFile.Load("mySound.wav")
```
Note that by default, the sound file will load all of its datas in memory. If you want the sound to be loaded only when played, set 'loadDatas' to false.
```
let soundFile = Audio.SoundFile.Load("mySound.wav", loadDatas = false)
```
Once you have a sound file, the simplest way to play it is by calling [Voice.play]. The sound will be played once, until the end, and will be destroyed.
```
Voice.play(soundFile)
```
To have more control, you could also use [Voice.create] then [Voice.Play] on the created sound. That way you will recieve a [Voice] object you can play with.
```
let voice = Voice.create(&soundFile)
voice.setVolume(0.5)
voice.play(Loop)
*/

#dependencies
{
    #import "core"
    #if #os == Swag.TargetOs.Windows
        #import "xaudio2"

    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.moduleVersion = #swagversion
        cfg.moduleRevision = #swagrevision
        cfg.moduleBuildNum = #swagbuildnum
        cfg.moduleNamespace = "Audio"
        cfg.backendKind = .DynamicLib
        cfg.docKind = .Api
        cfg.docOutputExtension = ".php"
        cfg.repoPath = "https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio"
    }
}