package require tcltest 2.5

set split_point [lsearch $argv --]
set program_args [lrange $argv [expr $split_point + 1] end]
set argv [lrange $argv 0 [expr $split_point - 1]]

::tcltest::configure {*}$::argv

package require Expect

set script_dir [file dirname [info script]]
source [file join $script_dir "expect_gdb_helpers.tcl"]

tcltest::test gdb-bytewrite {
    Writing a byte to the mmapped area should cause the program to stop and
    report that an anomaly was detected. The output values should be consistent
    with what was written to memory.
} -setup {
    lassign $program_args gdb_location cosmic_poll_location

    set cosmic_poll_info [start_gdb_cosmic_poll $gdb_location $cosmic_poll_location]
    set db_location [dict get $cosmic_poll_info db_location]
    set alloc_size [dict get $cosmic_poll_info alloc_size]
    set monitored_address [dict get $cosmic_poll_info monitored_address]

    exp_send -- "-exec-continue\r"
    expect_running

    sleep 2

    # This interrupt/continue is necessary in GDB 10, otherwise the memory write
    # will give the following error:
    #
    # ^error,msg="Cannot access memory at address 0x7ffff7fc3643
    #
    # This doesn't seem to be an issue in GDB 11
    exp_send -- "-exec-interrupt\r"

    expect {
        timeout {fail timeout}
        "\*stopped*signal-received"
    }

    set offset [expr {int(rand() * $alloc_size)}]
    set value [expr {int(rand() * 256)}]
    set in_memory_offset [expr {$monitored_address + $offset}]

    exp_send -- "-data-write-memory-bytes $in_memory_offset [format %02x $value]\r"
    expect_done
    exp_send -- "-exec-continue\r"
    expect_running

    expect {
        timeout {fail timeout}
        -re "Anomaly detected at offset (\[0-9a-f\]*) value (\[0-9a-f\]*)\r\n"
    }

    set detected_offset "0x$expect_out(1,string)"
    set detected_value "0x$expect_out(2,string)"
} -body {
    assert {$offset == $detected_offset} "Offset"
    assert {$value == $detected_value} "Value"
} -cleanup {
    file delete -- $db_location
}

exit_test
