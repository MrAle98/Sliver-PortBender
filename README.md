# Sliver-PortBender

An **extension to Sliver C2**, written as a **learning excercise**, that allows to **manage multiple redirections of traffic, from port to port, inside the same implant**. 
It maybe useful for performing **NTLM relaying in Sliver** in a similar way to what is described here https://rastamouse.me/ntlm-relaying-via-cobalt-strike/ for Cobalt-Strike. 
**The extension is a DLL wrapping the PortBender Reflective DLL** developed by praetorian-inc here: https://github.com/praetorian-inc/PortBender. Sliver-PortBender performs the same redirection task, 
performed by the original PortBender, but does everything **inside the same implant with no need to inject into other binaries**.

## How it works

The DLL manages multiple threads each one in charge of redirecting traffic from one port to another one. It keeps a map ID -> thread that allows to destroy/create/list the different
threads. For redirection task most of the code was taken from the original PortBender project and it was refactored in order to not crash the implant.
Even if the **backdoor** command appears as a possible argument, it is actually **not available**.

## How to build it

open the solution file using visual studio 2019, and build the x64 release (I didn't test the x86).

## how to load / install it

Until this is potentially included in the armory it can be built on a windows system and loaded manually in sliver.
Create a directory called sliver-extension and copy inside it SliverPortBender.dll, the binary previously compiled on windows with visual studio, and extension.json:
```
$ mkdir sliver-extension && cd sliver-extension
$ cp /path/to/extension.json .
$ cp /path/to/SliverPortBender.dll .
```

Install and load the extension in sliver-client:
```
> extension install /path/to/sliver-extension
> extension load /path/to/sliver-extension
```

