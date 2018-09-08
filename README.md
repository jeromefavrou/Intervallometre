
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
2. sudo apt-get install gphoto2 libgphoto2* git cmake
3. cd /opt
4. sudo git clone https://github.com/opencv/opencv.git
5. cd opencv
6. mkdir build
7. cd build
8. sudo cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..
9. sudo make
10. sudo make install
11. sudo apt-get autoremove
12. sudo apt-get autoclean
13. sudo rm -r /opt/opencv
