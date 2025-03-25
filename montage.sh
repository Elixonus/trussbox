read -r -e -p "create new bridges output? (y/n): " create_bridges
if ! [[ "$create_bridges" == "y" || "$create_bridges" == "Y" || "$create_bridges" == "n" || "$create_bridges" == "N" ]]
then
    echo "unrecognized input"
    exit 1
fi
read -r -e -p "create new pendulums output? (y/n): " create_pendulums
if ! [[ "$create_pendulums" == "y" || "$create_pendulums" == "Y" || "$create_pendulums" == "n" || "$create_pendulums" == "N" ]]
then
    echo "unrecognized input"
    exit 1
fi
mkdir -p tmp/montage
rm -rf tmp/montage/*
if [[ "$create_bridges" == "y" || "$create_bridges" == "Y" ]]
then
    ./bridges.sh
fi
if [[ "$create_pendulums" == "y" || "$create_pendulums" == "Y" ]]
then
    ./pendulums.sh
fi
mkdir -p tmp/montage/bridges/warren
echo "\
subtitles=1
center=(0.0 -0.4) lineheight=0.05 text=Warren Bridge
" | ./bin/subtitles tmp/montage/bridges/warren fsize=1920x1080
ffmpeg \
    -i tmp/bridges/warren/video.mp4 \
    -i tmp/bridges/warren/fdiagram.mp4 \
    -i tmp/montage/bridges/warren/subtitles.png \
    -filter_complex "[1:v]scale=640:360[y];[0:v][y]overlay[z];[z][2:v]overlay[v]" \
    -map "[v]" \
    -y tmp/montage/bridges/warren/video.mp4 \
    -loglevel error
mkdir -p tmp/montage/bridges/pratt
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.05 text=Pratt Bridge
" | ./bin/subtitles tmp/montage/bridges/pratt fsize=1920x1080
ffmpeg \
    -i tmp/bridges/pratt/video.mp4 \
    -i tmp/bridges/pratt/fdiagram.mp4 \
    -i tmp/montage/bridges/pratt/subtitles.png \
    -filter_complex "[1:v]scale=640:360[y];[0:v][y]overlay[z];[z][2:v]overlay[v]" \
    -map "[v]" \
    -y tmp/montage/bridges/pratt/video.mp4 \
    -loglevel error
mkdir -p tmp/montage/bridges/howe
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.05 text=Howe Bridge
" | ./bin/subtitles tmp/montage/bridges/howe fsize=1920x1080
ffmpeg \
    -i tmp/bridges/howe/video.mp4 \
    -i tmp/bridges/howe/fdiagram.mp4 \
    -i tmp/montage/bridges/howe/subtitles.png \
    -filter_complex "[1:v]scale=640:360[y];[0:v][y]overlay[z];[z][2:v]overlay[v]" \
    -map "[v]" \
    -y tmp/montage/bridges/howe/video.mp4 \
    -loglevel error
mkdir -p tmp/montage/bridges/parker
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.05 text=Parker Bridge
" | ./bin/subtitles tmp/montage/bridges/parker fsize=1920x1080
ffmpeg \
    -i tmp/bridges/parker/video.mp4 \
    -i tmp/bridges/parker/fdiagram.mp4 \
    -i tmp/montage/bridges/parker/subtitles.png \
    -filter_complex "[1:v]scale=640:360[y];[0:v][y]overlay[z];[z][2:v]overlay[v]" \
    -map "[v]" \
    -y tmp/montage/bridges/parker/video.mp4 \
    -loglevel error
mkdir -p tmp/montage/bridges/cambridge
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.05 text=Cambridge Bridge
" | ./bin/subtitles tmp/montage/bridges/cambridge fsize=1920x1080
ffmpeg \
    -i tmp/bridges/cambridge/video.mp4 \
    -i tmp/bridges/cambridge/fdiagram.mp4 \
    -i tmp/montage/bridges/cambridge/subtitles.png \
    -filter_complex "[1:v]scale=640:360[y];[0:v][y]overlay[z];[z][2:v]overlay[v]" \
    -map "[v]" \
    -y tmp/montage/bridges/cambridge/video.mp4 \
    -loglevel error
mkdir -p tmp/montage/pendulums/pendulum
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.05 text=Single Pendulum
" | ./bin/subtitles tmp/montage/pendulums/pendulum fsize=1920x1080
ffmpeg \
    -i tmp/pendulums/pendulum/video.mp4 \
    -i tmp/pendulums/pendulum/fdiagram.mp4 \
    -i tmp/montage/pendulums/pendulum/subtitles.png \
    -filter_complex "[1:v]scale=640:360[y];[0:v][y]overlay[z];[z][2:v]overlay[v]" \
    -map "[v]" \
    -y tmp/montage/pendulums/pendulum/video.mp4 \
    -loglevel error
mkdir -p tmp/montage/pendulums/doublependulum
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.05 text=Double Pendulum
" | ./bin/subtitles tmp/montage/pendulums/doublependulum fsize=1920x1080
ffmpeg \
    -i tmp/pendulums/doublependulum/video.mp4 \
    -i tmp/pendulums/doublependulum/fdiagram.mp4 \
    -i tmp/montage/pendulums/doublependulum/subtitles.png \
    -filter_complex "[1:v]scale=640:360[y];[0:v][y]overlay[z];[z][2:v]overlay[v]" \
    -map "[v]" \
    -y tmp/montage/pendulums/doublependulum/video.mp4 \
    -loglevel error
mkdir -p tmp/montage/pendulums/doublependulumroller
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.05 text=Double Pendulum with Roller Support
" | ./bin/subtitles tmp/montage/pendulums/doublependulumroller fsize=1920x1080
ffmpeg \
    -i tmp/pendulums/doublependulumroller/video.mp4 \
    -i tmp/pendulums/doublependulumroller/fdiagram.mp4 \
    -i tmp/montage/pendulums/doublependulumroller/subtitles.png \
    -filter_complex "[1:v]scale=640:360[y];[0:v][y]overlay[z];[z][2:v]overlay[v]" \
    -map "[v]" \
    -y tmp/montage/pendulums/doublependulumroller/video.mp4 \
    -loglevel error
mkdir -p tmp/montage/pendulums/decuplependulum
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.05 text=Decuple Pendulum
" | ./bin/subtitles tmp/montage/pendulums/decuplependulum fsize=1920x1080
ffmpeg \
    -i tmp/pendulums/decuplependulum/video.mp4 \
    -i tmp/pendulums/decuplependulum/fdiagram.mp4 \
    -i tmp/montage/pendulums/decuplependulum/subtitles.png \
    -filter_complex "[1:v]scale=640:360[y];[0:v][y]overlay[z];[z][2:v]overlay[v]" \
    -map "[v]" \
    -y tmp/montage/pendulums/decuplependulum/video.mp4 \
    -loglevel error
ffmpeg \
    -i tmp/montage/bridges/warren/video.mp4 \
    -i tmp/montage/bridges/pratt/video.mp4 \
    -i tmp/montage/bridges/howe/video.mp4 \
    -i tmp/montage/bridges/parker/video.mp4 \
    -i tmp/montage/bridges/cambridge/video.mp4 \
    -i tmp/montage/pendulums/pendulum/video.mp4 \
    -i tmp/montage/pendulums/doublependulum/video.mp4 \
    -i tmp/montage/pendulums/doublependulumroller/video.mp4 \
    -i tmp/montage/pendulums/decuplependulum/video.mp4 \
    -filter_complex "[0:v][1:v][2:v][3:v][4:v][5:v][6:v][7:v][8:v]concat=n=9:v=1[v]" \
    -map "[v]" \
    -y tmp/montage/video.mp4 \
    -loglevel error
rm -rf tmp/montage/bridges
rm -rf tmp/montage/pendulums
