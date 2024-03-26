#global namespace Time

// Provides a set of methods and properties that you can use to accurately measure elapsed time
public struct Stopwatch
{
    isStarted:          bool
    startTimeStamp:     u64
    elapsedTicks:       u64
}

public impl Stopwatch
{
    // Starts, or resumes, measuring elapsed time for an interval
    mtd start()
    {
        if !isStarted
        {
            isStarted      = true
            startTimeStamp = nowPrecise()
        }
    }

    // Stops measuring elapsed time for an interval
    mtd stop()
    {
        if isStarted
        {
            let endTimeStamp       = nowPrecise()
            let elapsedTicksPeriod = endTimeStamp - startTimeStamp
            elapsedTicks += elapsedTicksPeriod
            isStarted = false
        }
    }

    // Stops time interval measurement and resets the elapsed time to zero
    mtd reset()
    {
        elapsedTicks   = 0
        startTimeStamp = 0
        isStarted      = false
    }

    // Stops time interval measurement, resets the elapsed time to zero, and starts measuring elapsed time
    mtd restart()
    {
        elapsedTicks   = 0
        startTimeStamp = nowPrecise()
        isStarted      = true
    }

    // Gets the current elapsed time in milliseconds since the start
    mtd elapsedMillisecondsNow()->u32
    {
        let endTimeStamp       = nowPrecise()
        let elapsedTicksPeriod = endTimeStamp - startTimeStamp
        return acast ((elapsedTicksPeriod * 1000) / preciseFrequency())
    }

    // Gets the current elapsed time in milliseconds since the start
    mtd elapsedMicrosecondsNow()->u32
    {
        let endTimeStamp       = nowPrecise()
        let elapsedTicksPeriod = endTimeStamp - startTimeStamp
        return acast ((elapsedTicksPeriod * 1000000) / preciseFrequency())
    }

    // Gets the total elapsed time in milliseconds, after a call to 'stop'
    mtd elapsedMilliseconds()->u32
    {
        return acast ((elapsedTicks * 1000) / preciseFrequency())
    }

    // Gets the total elapsed time in microseconds, after a call to 'stop'
    mtd elapsedMicroseconds()->u32
    {
        return acast ((elapsedTicks * 1000000) / preciseFrequency())
    }

    // Measure until the end of the scope, and print the result in the console
    #[Swag.Mixin]
    func scopeMeasure(name: string = "stopwatch")
    {
        var #mixin0: Time.Stopwatch
        #mixin0.start()
        defer
        {
            #mixin0.stop()
            Console.printf("%: %ms\n", name, #mixin0.elapsedMilliseconds())
        }
    }
}
