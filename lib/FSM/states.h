#pragma once

enum State
{
        CLOSED_STATE,
        CLOSE_PRESSED,
        CLOSING,
        LOCKING,
        LOCKED,
        LOCK_PRESSED,
        UNLOCKING,

        OPEN_STATE,
        OPEN_PRESSED,
        OPENING,

        IDLE_STATE,
        ERROR,
        _, // all state
};