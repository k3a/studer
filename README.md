# ScomLib Extra

This code makes it easier to use Xcom-232i from Studer Innotec. 
When seen as textual data, Studer protocol looks like AA3650001000E0762A2110BDB00100000CCF2.

In the older branch [v1](../../tree/v1), I was constructing and reading the frames directly.
This newer branch is using the official scom library provided by Studer, extended by helper
functions and pre-allocated frame buffer.

That decouples the code from the serial port API and makes debugging easier.

The old branch should still work but this one should be preferred as I don't actively
use the old branch anymore.

You can use this code as you see fit, it is released under MIT license.

### Usage

Please see the [example app](example) and comments
in the [scomlib_extra.h](scomlib_extra/scomlib_extra.h) for the API usage.

### Contributing

Feel free to submit pull requests to improve the code, for example extending enums with object IDs.
The recommended editor to use is Visual Studio Code / VSCodium.

### Reference

The latest scom library and protocol spec can be downloaded
from [Software and Updates](https://www.studer-innotec.com/en/downloads/variotrack-series/) section.

Full list of object IDs can be found in "Technical specification - Xtender serial protocol appendix"
PDF in the same zip file.
