heavily inspired by Ange The Great's physics simulation

# Dampspring

## Preview

![Warren Truss Image](./preview.png)

[Warren Truss Video](./preview.mp4)

## Steps

### Setup

```$ ./setup.sh```

### Build

```$ ./build.sh```

### Run

```$ ./bridges.sh```

Creates solutions (a video and force data on each member and reactions on each support in the tmp directory) for each of the three trusses: warren, pratt, howe without further input.

**or**

```$ ./solvetruss.sh```

Creates solution (a video and force data on each member and reactions on each support) for a given problem containing input data for joints, members, supports, loads as well as other input data provided into standard input of the shell script (gravity, duration, frame rate, step rate, etc...).

Example stdin:


```
$ warren.txt
$ 9.8
$ 3.0
$ 3000.0
$ 60.0
$ 1920
$ 1080
$ 0.5
$ 0.125
$ 1.0
$ 1.0
$ tmp/warren
```

