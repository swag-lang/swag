/*
--- Part Two ---
It's no good - in this configuration, the amplifiers can't generate a large enough output signal to produce the thrust you'll need. The Elves quickly talk you through rewiring the amplifiers into a feedback for:

      O-------O  O-------O  O-------O  O-------O  O-------O
0 -+->| Amp A |->| Amp B |->| Amp C |->| Amp D |->| Amp E |-.
   |  O-------O  O-------O  O-------O  O-------O  O-------O |
   |                                                        |
   '--------------------------------------------------------+
                                                            |
                                                            v
                                                     (to thrusters)
Most of the amplifiers are connected as they were before; amplifier A's output is connected to amplifier B's input, and so on. However, the output from amplifier E is now connected into amplifier A's input. This creates the feedback for: the signal will be sent through the amplifiers many times.

In feedback for mode, the amplifiers need totally different phase settings: integers from 5 to 9, again each used exactly once. These settings will cause the Amplifier Controller Software to repeatedly take input and produce output many times before halting. Provide each amplifier its phase setting at its first input instruction; all further input/output instructions are for signals.

Don't restart the Amplifier Controller Software on any amplifier during this process. Each one should continue receiving and sending signals until it halts.

All signals sent or received in this process will be between pairs of amplifiers except the very first signal and the very last signal. To start the process, a 0 signal is sent to amplifier A's input exactly once.

Eventually, the software on the amplifiers will halt after they have processed the final for. When this happens, the last output signal from amplifier E is sent to the thrusters. Your job is to find the largest output signal that can be sent to the thrusters using the new phase settings and feedback for arrangement.

Here are some example programs:

Max thruster signal 139629729 (from phase setting sequence 9,8,7,6,5):

3,26,1001,26,-4,26,3,27,1002,27,2,27,1,27,26,
27,4,27,1001,28,-1,28,1005,28,6,99,0,0,5
Max thruster signal 18216 (from phase setting sequence 9,7,8,5,6):

3,52,1001,52,-5,52,3,53,1,52,56,54,1007,54,5,55,1005,55,26,1001,54,
-5,54,1105,1,12,1,53,54,53,1008,54,0,55,1001,55,1,55,2,53,55,53,4,
53,1001,56,-1,56,1005,56,6,99,0,0,0,0,10
Try every combination of the new phase settings on the amplifier feedback for. What is the highest signal that can be sent to the thrusters?
*/

using Core

private func run(op: &Array's32, input: const [2] s32, ip: &s32)->{ output: s32, stopped: bool }
{
    var output    = 0
    var phaseDone = ip != 0

    while op[ip] != 99
    {
        let imm0 = (op[ip] / 100) % 10
        let imm1 = (op[ip] / 1000) % 10
        let inst = op[ip] % 10

        var param0, param1 = 0's32
        if inst != 3 do
            param0 = imm0 ? op[ip + 1] : op[op[ip + 1]]
        if inst != 3 and inst != 4 do
            param1 = imm1 ? op[ip + 2] : op[op[ip + 2]]

        switch inst
        {
        case 1:
            op[op[ip + 3]] = param0 + param1
            ip += 4
        case 2:
            op[op[ip + 3]] = param0 * param1
            ip += 4
        case 3:
            op[op[ip + 1]] = phaseDone ? input[1] : input[0]
            ip += 2
            phaseDone = true
        case 4:
            output = param0
            ip += 2
            return {output, false}
        case 5:
            ip = param0 ? param1 : ip + 3
        case 6:
            ip = !param0 ? param1 : ip + 3
        case 7:
            op[op[ip + 3]] = param0 < param1 ? 1 : 0
            ip += 4
        case 8:
            op[op[ip + 3]] = param0 == param1 ? 1 : 0
            ip += 4
        }
    }

    return {output, true}
}

func day7B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "7.txt")
    let lines    = assume File.readAllLines(dataPath)

    var prg: Array's32
    foreach v in Tokenize.split(lines[0], ',''u8) do
        prg.add(assume StrConv.toS32(v))

    var phase = [5, 6, 7, 8, 9]
    var total = 0
    for
    {
        var op = [prg, prg, prg, prg, prg]
        var ip = [0, 0, 0, 0, 0]
        var input, cpt = 0
        for
        {
            let result = run(&op[cpt], [phase[cpt], input], &ip[cpt])
            total = Math.max(total, result.output)
            input = result.output
            cpt   = (cpt + 1) % 5
            if result.stopped and !cpt do
                break
        }

        if !Slice.nextPermutation(phase[ to ]) do
            break
    }

    //Console.print("total: ", total, "\n\n")
    @assert(total == 44282086)
}
