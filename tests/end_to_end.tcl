package require Expect

set script_dir [file dirname [info script]]
source [file join $script_dir "expect_helpers.tcl"]

lassign $argv gdb_location cosmic_poll_location

set cosmic_poll_info [start_cosmic_poll $gdb_location $cosmic_poll_location]

exp_send -- "-exec-continue\r"
expect_running

sleep 2
exp_send -- "-exec-interrupt\r"

expect {
    timeout {fail timeout}
    "\*stopped*signal-received"
}

set alloc_size [dict get $cosmic_poll_info alloc_size]
set monitored_address [dict get $cosmic_poll_info monitored_address]

set offset [expr {int(rand() * $alloc_size)}]
set value [expr {int(rand() * 256)}]
set in_memory_offset [expr {$monitored_address + $offset}]

exp_send -- "-data-write-memory-bytes $in_memory_offset [format %02x $value]\r"
expect_done
exp_send -- "-exec-continue\r"
expect_running

expect {
    timeout {fail timeout}
    -re {Anomaly detected at offset ([0-9a-f]*) value ([0-9a-f]*)}
}

set detected_offset "0x$expect_out(1,string)"
set detected_value "0x$expect_out(2,string)"

if {$offset != $detected_offset} {
    send_user "Got $detected_offset offset but should have been $offset offset\n"
    exit 1
}

if {$value != $detected_value} {
    send_user "Got $detected_value value but should have been $value value\n"
    exit 1
}

send_user "Success!\n"
exit 0
