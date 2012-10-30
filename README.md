# Continuation-passing-style (CPS) in C

This library provides an interface for writing continuation-passing-style (CPS)
code in C.  Each continuation is represented by an instance of `struct
cps_cont`, which contains a function pointer.  To invoke the continuation, we
call the function pointer.  The function takes in two continuation parameters:
the first is the continuation being invoked, and the second is the continuation
that the function should pass control to when it's done.

The continuation function doesn't allow any other parameters; information can
only be passed between continuations by embedding the `struct cps_cont` instance
in some larger struct, which should also contain any additional state that must
be passed into the continuation.

Since C isn't an inherently CPS-based language, we have to make the distinction
being regions of code that are using CPS, and regions that aren't.  “Normal” C
code can transfer control to CPS code by calling the `cps_run` function.  This
takes in a continuation object, which will be invoked, passing in an appropriate
`next` continuation to cause the `cps_run` function to return when the
continuation has finished.

Conversely, CPS code can return control the most recent “normal” C code region
by simply returning from the continuation function.  It can return a simple
integer status code, which can be used for basic error reporting.  When using a
compiler that implements sibling/tail-call optimizations, each region of CPS
code will only occupy one frame on the call stack.
