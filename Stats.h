#pragma once
struct Stats
{
    atomic<int> numFiles;
    atomic<int> numLines;
    atomic<int> numModules;
    int         numWorkers;
};
