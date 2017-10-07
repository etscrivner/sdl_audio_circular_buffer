# SDL Circular Audio Buffer

This repository contains an example of playing audio by writing raw data to a circular
buffer. Playback is provided by the SDL audio callback that reads from this circular buffer.

I couldn't find any good information on this online while debugging my own implementation, so I
thought I'd put this together to help others in the future.

## Building

The build has been tested on the following operating systems:

* macOS
* Linux

To build you should simply have to clone the repository and then do the following:

```
make all
```

This will attempt to build the SDL dependency provided along with the code in the repository, and
then build the application. If the build is successful, you can then run the example:

```
./build/circular_buffer
```
