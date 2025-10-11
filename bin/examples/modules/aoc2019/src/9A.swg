/*
--- Day 9: Sensor Boost ---
You've just said goodbye to the rebooted rover and left Mars when you receive a faint distress signal coming from the asteroid belt. It must be the Ceres monitoring station!

In order to lock on to the signal, you'll need to boost your sensors. The Elves send up the latest BOOST program - Basic Operation Of System Test.

While BOOST (your puzzle input) is capable of boosting your sensors, for tenuous safety reasons, it refuses to: so until the computer it runs on passes some checks to demonstrate it is a complete Intcode computer.

Your existing Intcode computer is missing one key feature: it needs support for parameters in relative mode.

Parameters in mode 2, relative mode, behave very similarly to parameters in position mode: the parameter is interpreted as a position. Like position mode, parameters in relative mode can be read from or written to.

The important difference is that relative mode parameters don't count from address 0. Instead, they count from a value called the relative base. The relative base starts at 0.

The address a relative mode parameter refers to is itself plus the current relative base. When the relative base is 0, relative mode parameters and position mode parameters with the same value refer to the same address.

For example, given a relative base of 50, a relative mode parameter of -7 refers to memory address 50 + -7 = 43.

The relative base is modified with the relative base offset instruction:

Opcode 9 adjusts the relative base by the value of its only parameter. The relative base increases (or decreases, if the value is negative) by the value of the parameter.
For example, if the relative base is 2000, then after the instruction 109,19, the relative base would be 2019. If the next instruction were 204,-34, then the value at address 1985 would be output.

Your Intcode computer will also need a few other capabilities:

The computer's available memory should be much larger than the initial program. Memory beyond the initial program starts with the value 0 and can be read or written like any other memory. (It is invalid to try to access memory at a negative address, though.)
The computer should have support for large numbers. Some instructions near the beginning of the BOOST program will verify this capability.
Here are some example programs that use these features:

109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99 takes no input and produces a copy of itself as output.
1102,34915192,34915192,7,4,7,99,0 should output a 16-digit number.
104,1125899906842624,99 should output the large number in the middle.
The BOOST program will ask for a single input; run it in test mode by providing it the value 1. It will perform a series of checks on each opcode, output any opcodes (and the associated parameter modes) that seem to be functioning incorrectly, and finally output a BOOST keycode.

Once your Intcode computer is fully functional, the BOOST program should report no malfunctioning opcodes when run in test mode; it should only output a single value, the BOOST keycode. What BOOST keycode does it produce?
*/

using Core

private func run(op: &Array's64, input: s64)->s64
{
    var output, ip, base = 0's64

    #[Swag.Mixin]
    func addr(offset: u64)->&s64
    {
        if offset >= op.count
        {
            op.grow(offset + 1)
            Memory.clear(op.buffer + op.count, (op.capacity - op.count) * #sizeof(s64))
            op.count = offset + 1
        }

        return op.buffer + offset
    }

    while op[ip] != 99
    {
        let mode0 = (op[ip] / 100) % 10
        let mode1 = (op[ip] / 1000) % 10
        let mode2 = (op[ip] / 10000) % 10
        let inst  = op[ip] % 10

        var param0, param1 = 0's64
        if inst != 3 do
            param0 = mode0 == 1 ? addr(ip + 1) : mode0 == 2 ? addr(op[ip + 1] + base) : addr(op[ip + 1])
        if inst != 3 and inst != 4 and inst != 9 do
            param1 = mode1 == 1 ? addr(ip + 2) : mode1 == 2 ? addr(op[ip + 2] + base) : addr(op[ip + 2])
        /*
        switch inst
        {
        case 1:   @print(ip, " ", "1: [", op[ip+3], "] = ", param0, " + ", param1, "\n")
        case 2:   @print(ip, " ", "2: [", op[ip+3], "] = ", param0, " * ", param1, "\n")
        case 3:   @print(ip, " ", "3: [", op[ip+1], "] = ", input, "\n")
        case 4:   @print(ip, " ", "4: out = ", param0, "\n")
        case 5:   @print(ip, " ", "5: if ", param0, " != 0 goto ", param1, "\n")
        case 6:   @print(ip, " ", "6: if ", param0, " == 0 goto ", param1, "\n")
        case 7:   @print(ip, " ", "7: [", op[ip+3], "] = ", param0, " < ", param1, "\n")
        case 8:   @print(ip, " ", "8: [", op[ip+3], "] = ", param0, " == ", param1, "\n")
        case 9:   @print(ip, " ", "9: base += ", param0, " (", base+param0, ")", "\n")
        default:  @print("?\n")
        }
*/
        switch inst
        {
        case 1:
            addr(op[ip + 3] + (mode2 == 2 ? base : 0)) = param0 + param1
            ip += 4
        case 2:
            addr(op[ip + 3] + (mode2 == 2 ? base : 0)) = param0 * param1
            ip += 4
        case 3:
            addr(op[ip + 1] + (mode0 == 2 ? base : 0)) = input
            ip += 2
        case 4:
            output = param0
            ip += 2
        case 5:
            ip = param0 ? param1 : ip + 3
        case 6:
            ip = !param0 ? param1 : ip + 3
        case 7:
            addr(op[ip + 3] + (mode2 == 2 ? base : 0)) = param0 < param1 ? 1 : 0
            ip += 4
        case 8:
            addr(op[ip + 3] + (mode2 == 2 ? base : 0)) = param0 == param1 ? 1 : 0
            ip += 4
        case 9:
            base += param0
            ip += 2
        }
    }

    return output
}

func day9A()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "9.txt")
    let lines    = assume File.readAllLines(dataPath)

    var prg: Array's64
    foreach v in Tokenize.split(lines[0], ',''u8) do
        prg.add(assume StrConv.toS64(v))

    let total = run(&prg, 1)
    //Console.print("total: ", total)
    @assert(total == 3013554615)
}
