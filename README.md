This project has a temporary name as "Blue Agent", it uses bluetooth as a proximity sensor to detect my phone when I am getting near to my office desk.  
It is still under construct, the final goal is to use this sensor to unlock and lock my Windows PCs, and this program runs on my raspberry pi.
So even when my pc is in sleep mode it can also wake the pc up through network.

### Installation ###

```bash
sudo apt-get install libbluetooth-dev
sudo apt-get install libssl-dev
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
./blueAgent -c config.json 
```

### ToDo ###
1. Add the feature to assign scheduled run time.
2. Abstracting agent class instead of using callback funcs directly.
3. Use cond_variable to practice concurrency.
4. Dig into bluetooth mesh protocol to see if we can access multiple devices to provide more accurate location.

***Cautions*** 

This project is not finished yet, wear your helmet.
