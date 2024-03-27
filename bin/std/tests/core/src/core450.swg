using Core

#test
{
    let str = assume File.readAllText(filePath("h1v1.jpg"))
    const N = 100

    {
        //Time.Stopwatch.scopeMeasure()
        loop N do
            discard Hash.City32.compute(str.toSlice())
    }

    {
        //Time.Stopwatch.scopeMeasure()
        loop N do
            discard Hash.Crc32.compute(str.toSlice())
    }
}
