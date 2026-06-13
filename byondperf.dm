var/_perf_mark = load_ext("byondperf", "byond:PerfMark")
var/_perf_elapsed = load_ext("byondperf", "byond:PerfElapsed")

/proc/perf_mark()
	return call_ext(_perf_mark)()

//#define perf_mark(...) call_ext(_perf_mark)()
#define perf_seconds(start, end) call_ext(_perf_elapsed)(start, end, 1)
#define perf_milliseconds(start, end) call_ext(_perf_elapsed)(start, end, 1000)
#define perf_microseconds(start, end) call_ext(_perf_elapsed)(start, end, 1000000)