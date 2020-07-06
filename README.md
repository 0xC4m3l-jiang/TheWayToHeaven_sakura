# The_way_to_heaven
Learning   在 `sakura` 大学学习 冲冲冲～ 

## 1、cool compiler 学习

学习 cool compiler pa2 pa3 pa4 pa5

Cool 环境

```bash
sudo apt-get install flex bison build-essential csh libxaw7-dev
sudo mkdir /usr/class
sudo chown $USER /usr/class
cd /usr/class
wget https://courses.edx.org/asset-v1:StanfordOnline+SOE.YCSCS1+1T2020+type@asset+block@student-dist.tar.gz
# 解压
tar -xf student-dist.tar.gz
# patch
ln -s /usr/class/cs143/cool ~/cool
PATH=/usr/class/cs143/cool/bin:$PATH
```

