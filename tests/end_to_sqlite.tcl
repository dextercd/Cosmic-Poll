package require tcltest 2.5

set split_point [lsearch $argv --]
set program_args [lrange $argv [expr $split_point + 1] end]
set argv [lrange $argv 0 [expr $split_point - 1]]

::tcltest::configure {*}$::argv

package require Expect
package require sqlite3

set script_dir [file dirname [info script]]
source [file join $script_dir "expect_gdb_helpers.tcl"]

tcltest::test sqlite {
    Check that the database contains the expected data after an anomaly was
    detected.
} -setup {
    lassign $program_args gdb_location cosmic_poll_location

    set cosmic_poll_info [start_gdb_cosmic_poll $gdb_location $cosmic_poll_location]

    set db_location [dict get $cosmic_poll_info db_location]
    set alloc_size [dict get $cosmic_poll_info alloc_size]
    set monitored_address [dict get $cosmic_poll_info monitored_address]

    exp_send -- "-exec-continue\r"
    expect_running

    set run_for 2
    sleep $run_for

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
        "Anomaly detected at offset * value *\r\n"
    }

    sqlite3 db $db_location
} -body {
    assert {[db eval {SELECT COUNT(*) FROM anomalies}] == 1} "There should only be one anomaly"
    db eval {SELECT * FROM anomalies} anomaly {
        assert {$anomaly(offset) == $offset} "Offset in db should be $offset"
        assert {$anomaly(value) == $value} "Value in db should be $value"
        assert {$anomaly(bytes) == $alloc_size} "Byte count in db should be $alloc_size"
    }

    assert {[db eval {SELECT COUNT(*) FROM active_times}] == 1} "Expected one active_times"
    db eval {SELECT * FROM active_times} active_time {
        assert {$active_time(seconds) >= $run_for} "Duration in db should be >= $run_for"
        assert {$active_time(seconds) <= 2 + $run_for} "Duration in db should be <= 2 + $run_for"
        assert {$active_time(bytes) == $alloc_size} "Byte count in db should be $alloc_size"
    }
} -cleanup {
    file delete -- $db_location
}

exit_test
