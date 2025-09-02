#global namespace Time

// Provides a set of methods and properties that you can use to accurately measure elapsed time
public struct Stopwatch
{
    isStarted:          bool     // True if the stopwatch is currently running
    startTimeStamp:     u64      // Timestamp when the stopwatch was last started
    elapsedTicks:       u64      // Accumulated elapsed time in ticks
}

public impl Stopwatch
{
    // Starts or resumes measuring elapsed time
    mtd start()
    {
        if !me.isStarted
        {
            me.isStarted      = true
            me.startTimeStamp = nowPrecise()
        }
    }

    // Stops measuring elapsed time and accumulates the result
    mtd stop()
    {
        if me.isStarted
        {
            let endTimeStamp       = nowPrecise()
            let elapsedTicksPeriod = endTimeStamp - me.startTimeStamp
            me.elapsedTicks += elapsedTicksPeriod
            me.isStarted = false
        }
    }

    // Resets the stopwatch and clears the elapsed time
    mtd reset()
    {
        me.elapsedTicks   = 0
        me.startTimeStamp = 0
        me.isStarted      = false
    }

    // Resets the stopwatch and immediately starts measuring elapsed time
    mtd restart()
    {
        me.elapsedTicks   = 0
        me.startTimeStamp = nowPrecise()
        me.isStarted      = true
    }

    // Returns the current elapsed time in milliseconds since the last start
    mtd elapsedMillisecondsNow()->u32
    {
        let endTimeStamp       = nowPrecise()
        let elapsedTicksPeriod = endTimeStamp - me.startTimeStamp
        return cast() ((elapsedTicksPeriod * 1000) / preciseFrequency())
    }

    // Returns the current elapsed time in microseconds since the last start
    mtd elapsedMicrosecondsNow()->u32
    {
        let endTimeStamp       = nowPrecise()
        let elapsedTicksPeriod = endTimeStamp - me.startTimeStamp
        return cast() ((elapsedTicksPeriod * 1000000) / preciseFrequency())
    }

    // Returns the total elapsed time in milliseconds, after calling `stop`
    mtd elapsedMilliseconds()->u32
    {
        return cast() ((me.elapsedTicks * 1000) / preciseFrequency())
    }

    // Returns the total elapsed time in microseconds, after calling `stop`
    mtd elapsedMicroseconds()->u32
    {
        return cast() ((me.elapsedTicks * 1000000) / preciseFrequency())
    }

    // Measures elapsed time for the current scope and prints the result when the scope ends
    #[Swag.Mixin]
    func scopeMeasure(name: string = "stopwatch")
    {
        var #uniq0: Time.Stopwatch
        #uniq0.start()
        defer
        {
            #uniq0.stop()
            Console.printf("%: %ms\n", name, #uniq0.elapsedMilliseconds())
        }
    }
}
