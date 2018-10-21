This project has a temporary name as "Blue Agent", it uses bluetooth as a proximity sensor to detect my phone when I am getting near to my cubic.  
It is still under construct, the final goal is to use this sensor to unlock and lock my Windows PCs, and this program runs on my raspberry pi.
so even when my pc is sleep mode it can also wake the pc up through network.

### Installation ###

```bash
sudo apt-get install libbluetooth-dev
```
Once CMake has been install navigate to the root of the project and issue the following commands:
```bash
git clone https://github.com/MiniSoda/cblue.git
cd cblue
mkdir build
cd build
cmake .. && make all
```


### Usage ###
```bash
./CBlue -b 12:34:56:78:90:AB
```

***Cautions*** 
This project is not finished yet, wear your helmet.
