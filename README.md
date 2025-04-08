[![Quality](https://github.com/Elixonus/trussbox/actions/workflows/quality.yml/badge.svg)](https://github.com/Elixonus/trussbox/actions/workflows/quality.yml)
[![Build](https://github.com/Elixonus/trussbox/actions/workflows/build.yml/badge.svg)](https://github.com/Elixonus/trussbox/actions/workflows/build.yml)
![Issues](https://img.shields.io/github/issues/Elixonus/trussbox)
![Language](https://img.shields.io/badge/Language-C23-blue)
![Platform](https://img.shields.io/badge/Platform-GNU/Linux-orange)
![License](https://img.shields.io/badge/License-MIT-red.svg)
![Code Size](https://img.shields.io/github/languages/code-size/Elixonus/trussbox)

# Trussbox

## Preview

[Montage Video (2K60FPS)](./previewmt.mp4)

[![Montage Image](./previewmt.png)](./previewmt.mp4)

[Montage (Parallel) Video (2K60FPS)](./previewmtpl.mp4)

[![Montage (Parallel) Image](./previewmtpl.png)](./previewmtpl.mp4)

[Warren Truss Bridge Video (2K60FPS)](./preview.mp4)

[![Warren Truss Bridge Image](./preview.png)](./preview.mp4)

[Warren Truss Bridge Force Diagram Video (2K60FPS)](./previewfd.mp4)

[![Warren Truss Bridge Force Diagram](./previewfd.png)](./previewfd.mp4)

## Steps

### Pre-Requirements

**Installation**: GNU/Linux and apt package manager

**Compilation**: GNU/Linux and gcc-14 C23 or higher

* Recommended distro: Ubuntu 24.04 or higher

**Execution**: GNU/Linux and minimum 2GB memory (for 2K60FPS)

### Setup

```./setup.sh```

### Format

```./format.sh```

### Build

```./build.sh```

### Run

```./bridges.sh```

Creates solutions (videos and force data on each member and reactions on each support in the tmp directory) for each of the five truss bridges: warren, pratt, howe, parker and cambridge without further input.

**or**

```./custom.sh```

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

## Usage

### Conventions

**Problem**

1. Joints Count *Header*
   * syntax: ```joints=count``` (newline)
   * number of joints in stream
2. Joints Lines *Body*
   * syntax: ```mass=float position=(float float) velocity=(float float)``` (newline)
   * each individual joint mass in kg, position vector in meters and velocity vector in meters per second
3. Members Count *Header*
   * syntax: ```members=count``` (newline)
   * number of members in stream
4. Members Lines *Body*
   * syntax: ```joint1=index joint2=index stiffness=float length0=float dampening=float``` (newline)
   * each individual member joints connection indices, stiffness of spring component in Newtons per meter, resting length in meters and dampening of damper component in Newton seconds per meter
5. Supports Count *Header*
   * syntax: ```supports=```
6. Supports Lines *Body*
7. Loads Count *Header*
8. Loads Lines *Body*



```
joints=integer
mass=
```

**Truss Solver**

* ```stdin```
* ```gravity```: gravitational acceleration of truss in meters per second squared
* ```timef```: elapsed simulation time in seconds
* ```srate```: frequency of simulation steps in time in Hz

```./bin/solvetruss gravity=gravity timef=timef srate=srate```

