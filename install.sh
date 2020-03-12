sudo apt-get install libbluetooth-dev libusb-1.0
cd host/libbtbb
make && sudo make install
cd ../
make && sudo make install
