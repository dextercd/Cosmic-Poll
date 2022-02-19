package require tcltest 2.5

set split_point [lsearch $argv --]
set program_args [lrange $argv [expr $split_point + 1] end]
set argv [lrange $argv 0 [expr $split_point - 1]]

::tcltest::configure {*}$::argv

package require Expect

set script_dir [file dirname [info script]]
source [file join $script_dir "test_helpers.tcl"]

lassign $program_args cosmic_poll_location

proc common_test_setup {name desc args} {
    tcltest::test $name $desc -setup {
        set db_location /tmp/cosmic_poll_[random_file_name].db

        global spawn_id
        set pid [ \
            spawn $::cosmic_poll_location --alloc-size 1KiB --check-interval 100ms \
                --db-location $db_location \
        ]

        # Make sure the program has started
        expect {
            timeout {fail timeout}
            -ex .
        }
    } {*}$args -cleanup {
        file delete -- $db_location
    }
}

proc check_normal_stop {} {
    expect {
        timeout {fail timeout}
        -ex "Stop signal received. Stopping..\r\n"
    }

    lassign [wait] {} {} {} exit_code
    assert {$exit_code == 0}
}

common_test_setup tty-C-c-stop {
    Program should stop when a stop signal is delivered via the TTY C-c key
    press.  The program should receive it as a SIGINT signal.
} -body {
    exp_send "\x03"
    check_normal_stop
}

common_test_setup sigint-stop {
    Program should stop after receiving SIGINT signal.
} -body {
    exec kill -s INT -- $pid
    check_normal_stop
}

common_test_setup sigterm-stop {
    Program should stop after receiving SIGTERM signal.
} -body {
    exec kill -s TERM -- $pid
    check_normal_stop
}

common_test_setup sighup-stop {
    Program should stop after receiving SIGHUP signal.
} -body {
    exec kill -s HUP -- $pid
    check_normal_stop
}

exit_test
