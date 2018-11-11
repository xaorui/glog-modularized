# glog-modularized
This project contains a modularized glog, a simple log, and cli-tools.
Throught the LAN cli, you can do the following things:
1、change the log level and switch for a module at
2、save and get param registered in the parameter server 

How to use it?
1、build
mkdir build
cd build
cmake ..
make

build result
install
├── bin
│   ├── cli_client
│   └── test_cli_srv
└── lib
    ├── libmlog.so -> libmlog.so.0.3.5
    ├── libmlog.so.0
    ├── libmlog.so.0.3.5 -> libmlog.so.0
    └── libtools.so
    
2、running
1) start the main process
  .//intstall/bin/test_cli_srv

2) run cli tool in a new terminal
  ./install/bin/cli_client

eg:  
ubuntu@localhost:/$ ./install/bin/cli_client 
Connected to '127.0.0.1'
<#>?
echo : echo testing
module : 
	add	add module
	config	get module config
	log	[module] (debug | info | notice | warning | error | fatal)
	sw	[module] on|off
param : 
	show		show all registed param des
	get	name	get registed param
	set	name	set registed param
<#>


