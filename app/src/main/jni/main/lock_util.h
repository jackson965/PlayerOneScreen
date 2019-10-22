//
// Created by ecar on 2019.10.22.
//

#ifndef AIPLAYER_LOCK_UTIL_H
#define AIPLAYER_LOCK_UTIL_H

#include <stdint.h>
#include "log.h"
#include "../sdl/include/SDL_mutex.h"


static inline void
mutex_lock(SDL_mutex *mutex) {
    if (SDL_LockMutex(mutex)) {
        LOGC("Could not lock mutex");
        abort();
    }
}

static inline void
mutex_unlock(SDL_mutex *mutex) {
    if (SDL_UnlockMutex(mutex)) {
        LOGC("Could not unlock mutex");
        abort();
    }
}

static inline void
cond_wait(SDL_cond *cond, SDL_mutex *mutex) {
    if (SDL_CondWait(cond, mutex)) {
        LOGC("Could not wait on condition");
        abort();
    }
}

static inline int
cond_wait_timeout(SDL_cond *cond, SDL_mutex *mutex, uint32_t ms) {
    int r = SDL_CondWaitTimeout(cond, mutex, ms);
    if (r < 0) {
        LOGC("Could not wait on condition with timeout");
        abort();
    }
    return r;
}

static inline void
cond_signal(SDL_cond *cond) {
    if (SDL_CondSignal(cond)) {
        LOGC("Could not signal a condition");
        abort();
    }
}

#endif //AIPLAYER_LOCK_UTIL_H
