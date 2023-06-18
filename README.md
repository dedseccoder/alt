# alt-test-utils

This repository is a test task for the alt Linux team. This CLI-app is using a custom-made library to export data from the alt public [API](https://rdb.altlinux.org/api/) and transforming it into a json file. To execute the app and compare data from two branches, run <br>

`applicationAlt [branch1] [branch2] [arch]` <br>

where `[branch1]` and `[branch2]` are the branches to compare, and `[arch]` is the architecture option (optional argument). Once executed, the app  will gather data from the API for both branches, generate a json file with the comparison, and print it to the command line.

[Branches and arches you can see here](https://packages.altlinux.org/ru/p10/)<br>
**Example:** `applicationAlt p10 p9 x86_64`

## installing

After cloning this repository, you will need to install two tools: `libcurl-devel` and `gcc` (GNU Compiler Collection) package. <br>
To install this CLI app and `libhttpUtils.so` library, run `sudo make install`. It will compile the code and place it in the right location according to the Filesystem Hierarchy Standard (FHS). <br>
Manual installation is also an option. In this case, just run `make`. It will build the application in the `bin` directory of the project. If there are any problems, run `make clean` to remove the created files.

### probably bug fixes

If you are encountering issues while using `sudo make install`, it's possible that your `/etc/ld.so.conf` configuration is not set up correctly. To fix this, open your `ld.so.conf` file using a text editor and add the following line:

```
include ld.so.conf.d/*.conf
/usr/local/lib (path to your libs)
```

This will ensure that any `.so` libraries placed in the `/usr/local/lib` directory are properly recognized. After making this change, you should be able to run `sudo make install` or `sudo ldconfig [path to libs]`**(for manual installation)** and successfully install your CLI app and `libhttpUtils.so` library. If you continue to experience issues, you may want to review your installation process and ensure that all dependencies have been installed and configured properly.
