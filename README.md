[![Quality](https://github.com/Elixonus/dampspring/actions/workflows/quality.yml/badge.svg)](https://github.com/Elixonus/dampspring/actions/workflows/quality.yml)
[![Build](https://github.com/Elixonus/dampspring/actions/workflows/build.yml/badge.svg)](https://github.com/Elixonus/dampspring/actions/workflows/build.yml)
![Issues](https://img.shields.io/github/issues/Elixonus/dampspring)
![Language](https://img.shields.io/badge/Language-C23-blue)
![Platform](https://img.shields.io/badge/Platform-GNU/Linux-orange)
[![License](https://img.shields.io/badge/License-MIT-red.svg)](https://opensource.org/licenses/MIT)
![Code Size](https://img.shields.io/github/languages/code-size/Elixonus/dampspring)

# Dampspring

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
