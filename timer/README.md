Get timer interrupts working on the EBAZ4205

*** Still in progress, not working yet.

Copied from /u1/Projects/OrangePi/Archive/inter_kyu

Nothing done yet, but will need to reviews the lds
file and especially the Makefile (pull my Makefile
from ../prf in this project.

For the Orange Pi this was a quick hack pulling all
kinds of stuff from Kyu.  My comments follow:

----

After pulling my hair out with some simple code
to demonstrate timer interrupts, I decided I could
help troubleshoot things by pulling in a few modules
from Kyu that would tell me about data aborts and give
traceback on exceptions, and such things.

So after a no holds barred hack-job editing session,
lo and behold it all just works!

So, we have here code that demonstrates using timer
interrupts on the Allwinner H3 using the ARM GIC.

This will be a useful stepping stone towards getting
Kyu running on the Orange Pi.

This code has no trouble with the timer running at
    10,000 Hz
