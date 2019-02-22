# Prototyping Delay-Aware V2X Safe Communications

![imgae](https://user-images.githubusercontent.com/29877872/52818029-e3237680-30a5-11e9-8428-835ce1037235.png)

### Prerequisites

```
 Linux 
```
### Installing

You need to get code files

```
git clone https://github.com/nicesick/Research-Lab.git
```

Change your current directory to /Research-Lab/resources/codes

```
cd /Research-Lab/resources/codes
```

Configure parameters before compilation

```
nano configuration.txt
```

Compilation

```
make setup
make client
make server
```

Execute them on your specific model (you probably need to move the files first)

```
sudo ./setup
sudo ./client
sudo ./server
```

If you want to set up different scheduling algorithms and different priority on them, please refer to *useful_commands.txt*

### Brief explanation

![intro](https://user-images.githubusercontent.com/29877872/53198944-0908c800-361e-11e9-843e-e2157689753f.png)

We assume that all nodes know other nodes's MAC addresses that will communicate with. When the client sends a message to the server, the message has its own index and the time when it was sent. After taking network latency, the server will receive the message and get the time when the message is received. Then, the server will calculate __Diff__ which is execution time by subtracting received time from the time just before sending back the decoded message to the client. When the client sends back the message, it will calculate __Message latency__. __Network latency__ is a result of subtraction __Diff__ from __Message latency.__

### *Repository structure*  
 ┌─── ___documents ::___  
 │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;└── __presentation ::__  
 │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;└── __references ::__  
 │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;└── kerenel_guide.txt (How we worked on kernel to adapt 802.11p patch file)  
 │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;└── project_proposal_ECE.pdf  
 │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;└── useful_commands.txt (Some commands you can refer)  
 │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;└── work_history.txt (What we worked everyday for 2 months)  
 ├─── ___resources ::___  
 │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;└── __code ::__  
 │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;└── 80211p_v4.9.patch  
 │&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;└── kernel.zip (the compressed files for our specific model)  
 └─── ___README.md___  

|**<center>Members</center>**|**<center>Advisors</center>**|
|:--------:|:--------:|
|<center> Hyunjae Lee, Chung-Ang University, South Korea </center>|<center>(Prof) Sebti Mouelhi (sebti.mouelhi@ece.fr)</center>|
|<center> Jihun Lim, Chung-Ang University, South Korea </center>|<center>(Prof) Rafik Zitouni (rafik.zitouni@ece.fr)</center>|
