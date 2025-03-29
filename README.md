# Dampspring

## Preview

[Montage Video (2K60FPS)](./previewmt.mp4)

![Montage Image](./previewmt.png)

[Warren Truss Bridge Video (2K60FPS)](./preview.mp4)

![Warren Truss Bridge Image](./preview.png)

[Warren Truss Bridge Force Diagram Video (2K60FPS)](./previewfd.mp4)

![Warren Truss Bridge Force Diagram](./previewfd.png)

## Steps

### Pre-Requirements

**Compilation**: GNU + Linux and gcc-14 C23 or higher

**Pipeline**: GNU + Linux

**Execution**: minimum 2GB memory (for 2K60FPS)


### Setup

```$ ./setup.sh```

### Format

```$ ./format.sh```

### Build

```$ ./build.sh```

### Run

```$ ./bridges.sh```

Creates solutions (videos and force data on each member and reactions on each support in the tmp directory) for each of the five truss bridges: warren, pratt, howe, parker and cambridge without further input.

**or**

```$ ./custom.sh```

Creates solution (videos and force data on each member and reactions on each support) for a given problem containing input data for joints, members, supports, loads as well as other input data provided into standard input of the shell script (gravity, duration, frame rate, step rate, etc...).

Example stdin:

```
$ warren.txt
$ 9.8
$ 3.0
$ 10000.0
$ 60.0
$ 1920
$ 1080
$ 0.5
$ 0.125
$ 1.0
$ 1.0
$ tmp/warren
```

**or**

```$ ./montage.sh```
