/mob/verb/call_perf()
    var/start = perf_mark()

    for (var/i in 1 to 1000000)
        var/x = i*i
    var/end = perf_mark()
    var/elapsed = perf_milliseconds(start, end)
    usr << "Loop took [elapsed] ms"