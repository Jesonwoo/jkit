function msec2string(t) {
    if (t <= 0)
        return "00:00:00:000"
    var mm = t%1000         // get msec
    t = Math.floor(t/1000)
    var s = t%60            // get second
    t = (t-s)/60
    var m = t%60            // get minutes
    var h = (t-m)/60        // get hours
    if (s < 10) { s = "0" + s }
    if (m < 10) { m = "0" + m }
    if (h < 10) { h = "0" + h }
    if(mm < 100) { mm = "0" + mm }
    else if(mm < 10) { mm = "00" + mm }
    return h + ":" + m + ":" + s + ":" + mm
}
