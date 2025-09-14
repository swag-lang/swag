using Core, Audio

#test
{
    let files = assume Directory.enumerateFiles(dataPath(), matchExtension: ".wav")
    foreach f in files
    {
        var _x = assume SoundFile.load(f.fullname)
    }
}
