#pragma once

// Number of lines of code printed for an error
static const int REPORT_NUM_CODE_LINES = 3;

struct SourceLocation
{
    int line;
    int column;
    int seekStartLine[REPORT_NUM_CODE_LINES];
};
