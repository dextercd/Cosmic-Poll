package require Expect

set script_dir [file dirname [info script]]
source [file join $script_dir "test_helpers.tcl"]

set gdb_prompt "(gdb) \r\n"

proc expect_prompt {} {
    expect {
        timeout {fail timeout}
        $::gdb_prompt
    }
}

proc expect_done {{prefix ""}} {
    expect {
        timeout {fail timeout}
        -ex "$prefix^done"
    }
}

proc expect_running {{prefix ""}} {
    expect {
        timeout {fail timeout}
        -ex "$prefix^running"
    }
}

# alloc_size must be <= 256 MiB on GitHub Actions according to this doc
# https://github.com/actions/virtual-environments/pull/1463
# But in my experience it must be even lower than that.

proc start_cosmic_poll {
        {gdb_command gdb}
        {cosmic_poll_command cosmic_poll}
        {alloc_size 65536}
        {check_interval 100ms}
} {
    global spawn_id
    set db_location /tmp/cosmic_poll_[random_file_name].db
    spawn $gdb_command --interpreter=mi3 --args      \
                $cosmic_poll_command                 \
                    --alloc-size $alloc_size         \
                    --check-interval $check_interval \
                    --db-location $db_location

    expect_prompt

    exp_send -- "-gdb-set mi-async on\r"
    expect_done
    exp_send -- "-gdb-set non-stop on\r"
    expect_done

    exp_send -- "-break-insert main\r"
    expect_done
    exp_send -- "-exec-run\r"
    expect_running

    expect {
        timeout {fail timeout}
        {\*stopped*reason="breakpoint-hit"*bkptno="1"}
    }

    exp_send -- "-break-insert mmap\r"
    expect_done
    exp_send -- "-exec-continue\r"
    expect_running

    expect {
        timeout {fail timeout}
        {\*stopped*reason="breakpoint-hit"*bkptno="2"}
    }

    exp_send -- "-exec-finish\r"
    expect_running

    expect {
        timeout {fail timeout}
        -ex "*stopped"
    }

    exp_send -- "-data-list-register-values x rax\r"

    expect {
        timeout {fail timeout}
        -re {\^done,register-values=.*value="([^"]*)"}
    }

    set monitored_address $expect_out(1,string)

    return [dict create                          \
        spawn_id            $spawn_id            \
        gdb_command         $gdb_command         \
        cosmic_poll_command $cosmic_poll_command \
        alloc_size          $alloc_size          \
        check_interval      $check_interval      \
        monitored_address   $monitored_address   \
        db_location         $db_location         \
    ]
}
