    .syntax unified
    .thumb

    /*--------------------------------------
     * Valvano helper routines
     *--------------------------------------*/

    .global EnableInterrupts
EnableInterrupts:
    cpsie i
    bx lr

    .global DisableInterrupts
DisableInterrupts:
    cpsid i
    bx lr

    .global StartCritical
StartCritical:
    mrs r0, primask
    cpsid i
    bx lr

    .global EndCritical
EndCritical:
    msr primask, r0
    bx lr

    .global WaitForInterrupt
WaitForInterrupt:
    wfi
    bx lr
