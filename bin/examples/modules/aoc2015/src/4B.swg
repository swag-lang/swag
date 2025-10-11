/*
--- Part Two ---
Now find one that starts with six zeroes.
*/

using Core

private var best: u64 = Swag.U64.Max

func day4B()
{
    #if #cfg == "release"
    {

        if @bytecode do
            return

        //Time.Stopwatch.scopeMeasure()
        assume Jobs.setNumWorkers()

        struct ForJob
        {
            using base:     Jobs.Job
            start:          u64
            result:         u64 = Swag.U64.Max
        }

        var sliceJobs: Array'ForJob
        sliceJobs.resize(Jobs.getNumWorkers())

        func jobEntry(jobData: *void)
        {
            let oneJob = cast(*ForJob) jobData
            var index  = oneJob.start
            let offset = Jobs.getNumWorkers()
            for
            {
                // Another job is better than us, just quit
                if Atomic.get(&best) < index do
                    return

                let str = Format.toString("yzbqklnj%", index)
                let v   = Hash.Md5.compute(str)
                if v[0] == 0 and v[1] == 0 and v[2] == 0
                {
                    Atomic.exchange(&best, index)
                    oneJob.result = index
                    return
                }

                index += offset
            }
        }

        foreach &j in sliceJobs
        {
            j.base.lambda = &jobEntry
            j.data        = j
            j.start       = @index
            Jobs.scheduleJob(j)
        }

        var total: u64 = Swag.U64.Max
        foreach &j in sliceJobs
        {
            Jobs.waitJob(j)
            total = Math.min(total, j.result)
        }

        //Console.print("total: ", total)
        @assert(total == 9962624)
    }
}
