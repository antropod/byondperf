#define DLL "byondperf"

var/_perf_mark = load_ext(DLL, "byond:PerfMark")
var/_perf_elapsed = load_ext(DLL, "byond:PerfElapsed")
var/_memory_stats = load_ext(DLL, "byond:MemoryStats")
#define memory_stats call_ext(_memory_stats)

#define perf_mark(...) call_ext(_perf_mark)()
#define perf_seconds(start, end) call_ext(_perf_elapsed)(start, end, 1)
#define perf_milliseconds(start, end) call_ext(_perf_elapsed)(start, end, 1000)
#define perf_microseconds(start, end) call_ext(_perf_elapsed)(start, end, 1000000)

#undef DLL
