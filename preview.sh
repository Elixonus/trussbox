read -r -e -p "make montage (y/n): " do_montage
if ! [[ "$do_montage" == "y" || "$do_montage" == "Y" || "$do_montage" == "n" || "$do_montage" == "N" ]]
then
    echo "unrecognized input"
    exit 1
fi
mkdir -p tmp/preview
rm -rf tmp/preview/*
cat warren.txt | sed "s/dampening=3.0e2/dampening=3.0e1/g" > warrenld.txt
echo "\
warrenld.txt
9.8
3.0
10000.0
60.0
1920
1080
0.5
0.125
1.0
1.0
tmp/preview
" | ./custom.sh
rm -f warrenld.txt
if [[ "$do_montage" == "y" || "$do_montage" == "Y" ]]
then
    ./montage.sh
fi
cp tmp/preview/video.mp4 preview.mp4
ffmpeg \
    -i tmp/preview/video.mp4 \
    -vf "fps=30,scale=960:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" \
    -y preview.gif \
    -loglevel error
cp tmp/preview/fdiagram.png previewfd.png
cp tmp/montage/video.mp4 previewmt.mp4
ffmpeg \
    -i tmp/montage/video.mp4 \
    -vf "fps=30,scale=960:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" \
    -y previewmt.gif \
    -loglevel error
