using Audio

#test
{
    assume Audio.createEngine()
    defer Audio.destroyEngine()

    assume
    {
        var x     = SoundFile.load(sndPath("pacman_chomp.wav"))
        let voice = Voice.create(&x)
        voice.setVolume(0)
        voice.play()
    }

    while voice.isPlaying()
    {
    }
    voice.destroy()
}
