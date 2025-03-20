./bridges.sh
./pendulum.sh
mkdir -p tmp/montage
rm -rf tmp/montage/*
mkdir -p tmp/montage/bridges/warren
ffmpeg -i tmp/bridges/warren/video.mp4 \
       -i tmp/bridges/warren/fdiagram.png \
       -filter_complex "[1:v]scale=640:360[z];[0:v][z]overlay[v]" \
       -map "[v]" \
       -y tmp/montage/bridges/warren/video.mp4 \
       -loglevel error
mkdir -p tmp/montage/bridges/pratt
ffmpeg -i tmp/bridges/pratt/video.mp4 \
       -i tmp/bridges/pratt/fdiagram.png \
       -filter_complex "[1:v]scale=640:360[z];[0:v][z]overlay[v]" \
       -map "[v]" \
       -y tmp/montage/bridges/pratt/video.mp4 \
       -loglevel error
mkdir -p tmp/montage/bridges/howe
ffmpeg -i tmp/bridges/howe/video.mp4 \
       -i tmp/bridges/howe/fdiagram.png \
       -filter_complex "[1:v]scale=640:360[z];[0:v][z]overlay[v]" \
       -map "[v]" \
       -y tmp/montage/bridges/howe/video.mp4 \
       -loglevel error
mkdir -p tmp/montage/bridges/parker
ffmpeg -i tmp/bridges/parker/video.mp4 \
       -i tmp/bridges/parker/fdiagram.png \
       -filter_complex "[1:v]scale=640:360[z];[0:v][z]overlay[v]" \
       -map "[v]" \
       -y tmp/montage/bridges/parker/video.mp4 \
       -loglevel error
mkdir -p tmp/montage/pendulums/pendulum
ffmpeg -i tmp/pendulums/pendulum/video.mp4 \
       -i tmp/pendulums/pendulum/fdiagram.png \
       -filter_complex "[1:v]scale=640:360[z];[0:v][z]overlay[v]" \
       -map "[v]" \
       -y tmp/montage/pendulums/pendulum/video.mp4 \
       -loglevel error
mkdir -p tmp/montage/pendulums/doublependulum
ffmpeg -i tmp/pendulums/doublependulum/video.mp4 \
       -i tmp/pendulums/doublependulum/fdiagram.png \
       -filter_complex "[1:v]scale=640:360[z];[0:v][z]overlay[v]" \
       -map "[v]" \
       -y tmp/montage/pendulums/doublependulum/video.mp4 \
       -loglevel error
mkdir -p tmp/montage/pendulums/doublependulumroller
ffmpeg -i tmp/pendulums/doublependulumroller/video.mp4 \
       -i tmp/pendulums/doublependulumroller/fdiagram.png \
       -filter_complex "[1:v]scale=640:360[z];[0:v][z]overlay[v]" \
       -map "[v]" \
       -y tmp/montage/pendulums/doublependulumroller/video.mp4 \
       -loglevel error
mkdir -p tmp/montage/pendulums/decuplependulum
ffmpeg -i tmp/pendulums/decuplependulum/video.mp4 \
       -i tmp/pendulums/decuplependulum/fdiagram.png \
       -filter_complex "[1:v]scale=640:360[z];[0:v][z]overlay[v]" \
       -map "[v]" \
       -y tmp/montage/pendulums/decuplependulum/video.mp4 \
       -loglevel error
ffmpeg -i tmp/montage/bridges/warren/video.mp4 \
       -i tmp/montage/bridges/pratt/video.mp4 \
       -i tmp/montage/bridges/howe/video.mp4 \
       -i tmp/montage/bridges/parker/video.mp4 \
       -i tmp/montage/pendulums/pendulum/video.mp4 \
       -i tmp/montage/pendulums/doublependulum/video.mp4 \
       -i tmp/montage/pendulums/doublependulumroller/video.mp4 \
       -i tmp/montage/pendulums/decuplependulum/video.mp4 \
       -filter_complex "[0:v][1:v][2:v][3:v][4:v][5:v][6:v][7:v]concat=n=8:v=1[v]" \
       -map "[v]" \
       -y tmp/montage/video.mp4 \
       -loglevel error
rm -rf tmp/montage/bridges
rm -rf tmp/montage/pendulums
