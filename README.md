# NevolutionX
Original Xbox dashboard created with nxdk.

## Motivation
As the XboxDev community grew, the need for an open-source, nxdk based dashboard arose. There are other dashboard alternatives i.e. XBMC but none of them are built using nxdk.

## Features
- [x] Basic Menu
- [x] Build-in FTP server
	- [x] IPv4 & IPv6
	- [ ] TLS
- [x] Application launcher
- [x] DVD launcher
- [ ] Themes

## Building with nxdk
### Build
In order to build NevolutionX you'll need to install [nxdk](https://github.com/XboxDev/nxdk) first.
To build NevolutionX you clone the repository, check out the submodules, and run `make`. You may need to adjust the installation location of nxdk in the `Makefile` to your needs. The default is `NXDK_DIR = $(CURDIR)/../nxdk` which expects nxdk right next to NevolutionX.

```sh
git clone https://github.com/dracc/NevolutionX.git --recursive
cd NevolutionX/
make
```

The xbox executable(`default.xbe`) will be inside the newly created `bin/`:
```sh
ls bin/
480.png 720.png default.xbe vegur.ttf
```

### Installation
Simply copy `default.xbe` and all the assets onto your xbox.
There is no further configuration required. The FTP-server will start automatically with NevolutionX, you'll only need to provide a link and a DHCP-server. The default user credentials are `xbox`/`xbox`.

## Building with CMake (Linux target)
TODO: Document what parts of the Linux target are not supported.
TODO: Document installation for the Linux target, if applicable.
`cmake -S . -B build && cmake --build build --verbose`

## Credits
This software is built on top of other awesome projects:
- The new xbox development kit [nxdk](https://github.com/XboxDev/nxdk)
- Visit the lovely [XboxDev](https://github.com/XboxDev/XboxDev) community

## License
 NevolutionX is published under the MIT License. See [LICENSE](LICENSE) for more information.
 MIT © 2019 Lucas Eriksson

