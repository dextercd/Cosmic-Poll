proc assert {cond {msg "assertion failed"}} {
    if {![uplevel 1 expr $cond]} {error $msg}
}

proc fail reason {
    error "Failed: $reason\n"
    exit 1
}

proc random_file_name {} {
    return [expr int(999999999 * rand())]
}
