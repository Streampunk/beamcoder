FROM cimg/node:16.14

# install FFmpeg
RUN sudo apt-get update \
  && sudo apt-get install software-properties-common \
  && sudo add-apt-repository ppa:savoury1/ffmpeg4 \
  && sudo add-apt-repository ppa:savoury1/ffmpeg5 \
  && sudo apt-get update \
  && sudo apt-get upgrade && sudo apt-get dist-upgrade \
  && sudo apt-get install libavcodec-dev libavformat-dev libavdevice-dev libavfilter-dev libavutil-dev libpostproc-dev libswresample-dev libswscale-dev

# delete all the apt list files since they're big and get stale quickly
RUN sudo rm -rf /var/lib/apt/lists/*
# this forces "apt-get update" in dependent images, which is also good
