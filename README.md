
# Intervallometre
-----------------
* installation automatique linux_64 (ex pour ubuntu):
-----------------
1. chmod +x ./install.linux64
2. sudo ./install.linux64 (peut prendre un certains temps)
3. chmod +x ./Intervallometre
4. ./Intervallometre

----------------------
* installation manuel linux_64 (ex pour ubuntu): (peut prendre un certains temps)
----------------------
1. sudo apt-get update
2. sudo apt-get install gphoto2 libgphoto2* git cmake python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev build-essential
3. sudo git clone https://github.com/opencv/opencv.git
4. cd opencv
5. mkdir build
6. cd build
7. sudo cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D BUILD_EXAMPLES=ON -D WITH_QT=ON -D WITH_OPENGL=ON
8. sudo make
9. sudo make install
10. sudo apt-get autoremove
11. sudo apt-get autoclean
12. cd ..
13. cd ..
14. sudo rm -r opencv
15. chmod +x ./Intervallometre
16. ./Intervallometre
