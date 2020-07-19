#include "pch.h"
#include "BackendC.h"
#include "Module.h"
#include "ByteCode.h"
#include "AstNode.h"

bool BackendC::emitDataSegment(OutputFile& bufferC, DataSegment* dataSegment, int precompileIndex)
{
    if (dataSegment == &module->mutableSegment)
        emitSeparator(bufferC, "MUTABLE SEGMENT");
    else if (dataSegment == &module->constantSegment)
        emitSeparator(bufferC, "CONSTANT SEGMENT");
    else
        emitSeparator(bufferC, "BSS SEGMENT");

    if (!dataSegment->buckets.size())
        return true;
    if (!dataSegment->totalCount)
        return true;

    if (precompileIndex != 0)
    {
        if (dataSegment == &module->mutableSegment)
            CONCAT_FIXED_STR(bufferC, "extern swag_uint8_t __mutableseg[];\n");
        else if (dataSegment == &module->constantSegment)
            CONCAT_FIXED_STR(bufferC, "extern swag_uint8_t __bssseg[];\n");
        else
            CONCAT_FIXED_STR(bufferC, "extern swag_uint8_t __constantseg[];\n");
    }
    else
    {
        auto segSize = dataSegment->buckets.size();
        if (dataSegment == &module->bssSegment)
        {
            CONCAT_STR_1(bufferC, "swag_uint8_t __bssseg[", dataSegment->totalCount, "];\n");
        }
        else
        {
            if (dataSegment == &module->mutableSegment)
                CONCAT_FIXED_STR(bufferC, "swag_uint8_t __mutableseg[] = {\n");
            else
                CONCAT_FIXED_STR(bufferC, "swag_uint8_t __constantseg[] = {\n");

            for (int bucket = 0; bucket < segSize; bucket++)
            {
                int  count = (int)dataSegment->buckets[bucket].count;
                auto pz = dataSegment->buckets[bucket].buffer;
                int  cpt = 0;
                while (count--)
                {
                    bufferC.addString(to_string(*pz));
                    bufferC.addChar(',');
                    pz++;
                    cpt = (cpt + 1) % 20;
                    if (cpt == 0)
                        bufferC.addEol();
                }
            }

            CONCAT_FIXED_STR(bufferC, "\n};\n");
        }
    }

    return true;
}