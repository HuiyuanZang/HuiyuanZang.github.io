# How to upgrade cuda toolkit on system76 laptop
The nvidia graphic card used in my system76 laptop is GeForce RTX3060. But the nvidia driver is 515.05.01-0ubuntu1. I want to upgrade the latest version. One more thing is that I want to install latest cuda toolkit based on latest cuda driver.


```shell
$ nvidia-smi -L
GPU 0: NVIDIA GeForce RTX 3060 Laptop GPU (UUID: GPU-a90d2dce-ce0f-2ffa-7374-c4d6ae7edf7b)
```

## Disable system76 source.list when installing new cuda toolkit
Comment out the system76 URL in /etc/apt/sources.list.d/system76-dev-ubuntu-statble-jammy.list temporarily like
```shell
vim /etc/apt/sources.list.d/system76-dev-ubuntu-stable-jammy.list 

#deb https://ppa.launchpadcontent.net/system76-dev/stable/ubuntu/ jammy main
# deb-src https://ppa.launchpadcontent.net/system76-dev/stable/ubuntu/ jammy main

```

## Install new cuda-toolkit 

Go to https://developer.nvidia.com/cuda-downloads?target_os=Linux&target_arch=x86_64&Distribution=Ubuntu&target_version=22.04&target_type=deb_network
```shell
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-keyring_1.0-1_all.deb
sudo dpkg -i cuda-keyring_1.0-1_all.deb
sudo apt-get update
sudo apt-get -y install cuda
```
then reboot device

## Check status of new cuda toolkit

please refer to [Installing CUDA and cuDNN in Ubuntu 22.04 for deep learning](https://medium.com/@juancrrn/installing-cuda-and-cudnn-in-ubuntu-20-04-for-deep-learning-dad8841714d6)


## Reference
- [1][Installing CUDA and cuDNN in Ubuntu 22.04 for deep learning](https://medium.com/@juancrrn/installing-cuda-and-cudnn-in-ubuntu-20-04-for-deep-learning-dad8841714d6)