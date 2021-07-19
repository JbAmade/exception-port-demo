# exception-port-demo
A simple program to demonstrate how to set an exception handler on Mac OS.

This program is made of 2 parts :
the "child" program, which crashes on purpose
the "exception" program : it calls the child, registers a Mach port to receive the EXC_BAD_ACCESS exception message caused by the child's crash and tells you when/if it has received it. 

## Be careful !
1) The parent doesn't actually handle the child's crash. One you close the parent, the child's crash won't be handled so you'll find a nice crash log coming from "child" in your console.
2) I didn't include compiled versions of the "child" and "exception" programs. Make sure to compile both of them before attemtping to run them.
3) In order to get the child's task port, we need to call `task_for_pid()`. However, because of SIP on Mac OS, we can't do this if we're not `root`. because adding the `task_for_pid-allow` entitlement to "exceptions" or `get-task-allow` on "child" will make `zsh` kill them. that's why we need to give "exceptions" debugging capabilities. Long stody short, you'll be prompted for your password because "exceptions" need to run as `root`.

## Sources and acknowledgments
The "heart" of the program comes from jailbreak techniques. This was a test to try it on Mac first.

Thanks to [@K0stad1n](https://github.com/K0stad1n) for making this porgram work both on Intel and M1 thanks to the pre-processor condition ! It's usefull, so that you don't have to care about your architecture. Just compile !
