using Core, Audio

#test
{
    var files = assume Directory.enumerateFiles(dataPath(), matchExtension: ".wav")
    foreach f in files
    {
        var _x = assume SoundFile.load(f.fullname)
    }
}
