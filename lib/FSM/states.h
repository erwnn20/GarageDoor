#pragma once

enum State
{
        CLOSED,
        CLOSE_PRESSED,
        CLOSING,
        LOCKED,
        LOCK_PRESSED,

        OPEN,
        OPEN_PRESSED,
        OPENING,

        IDLE,
        ERROR,
        _, // all state
};