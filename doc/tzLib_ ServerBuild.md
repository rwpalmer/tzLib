
## Creating a Virtual HTTP Server

  This document describes how tzLib's author created a virtual HTTP server for
  developing and testing tzLib, and is provided to help those who might wish
  to run tzLib using their own server. 
  
  There are dozens of other ways to create the required server. The author
  selected this approach becuse it is easy, free,  and can be used on computers 
  runing Windows, Linux, Mac OS 10 (64-bit, or later versions of 64-bit 
  Solaris. 
  
  The author built his server on the laptop that he uses every day. It is a 
  Lenovo ThinkPad with a 1.6GHz Intel i7 quad-core processor with 10GB of RAM, 
  and a 500GB hard disk. The Thinkpad runs a Windows 7 64-bit OS. The server
  runs as a virtual machine, in the background, and does not impact normal
  usage. 
  
### Step 1: Download and Install Oracle's VirtualBox Software 
  As mentioned above, a version of VirtualBox is available for numerous 
  operating systems. The software can be downloaded from:
  http://www.oracle.com/technetwork/server-storage/virtualbox/downloads/index.html
  
  VirtualBox installation is simple, and does not take long. 
  
### Step 2: Download Ubuntu Server
  The server version makes it easy to build the server that we need, and it 
  eliminates all the clutter that comes with desktop versions. The .ISO image
  can be downloaded from:
  https://www.ubuntu.com/download/server
  
  Don't burn a CD, unless you want one for other reasons. It is much faster
  to install directly from the ISO. 
  
### Step 3: Create the Virtual Machine
    a. Start the VirtualBox Manager
    b. Select "Machine" and "New" from the menu at the top of the page and 
       follow the prompts.
         - Be sure to select OS Type: Linux, and Version: Ubuntu(64=bit)
         - Default memory size is fine, and can be changed later
         - The default disk file type will be fine. The 'disk file' will
		   look like a file to your computer's OS, but will look like a
           physical hard-disk to your virtual machine.
         - 'Dynamically allocated' storage will be fine for this server
           because this will only use the disk space it requires, and that
           will be relatively static.
         - A virtual hard disk size (limit) of 30GB or more would be the 
           author's recommendation. 
       When you finish, you will see your virtual machine listed in the left
       panel of the VirtualBox Manager.
    c. Right-click on your virtual machine and select 'Settings'
         - Under 'Network', change Attached to: 'bridged adapter' so your
		   virtual machine can see your computer's network.
         - Under 'USB', select a USB controller version to match your
           computer's USB ports.
         - Under 'Storage', you want to mount the Ubuntu ISO image on your 
           virtual machine's virtual CD-ROM drive. This is not intuitive, 
		   so read carefully:
              - Click on the cd-rom image in the Storage Tree panel.
			  - Then click on the cd-rom image that appears in the upper
                right corner, under Attributes,  to select the Ubuntu ISO
                that you downloaded earlier ... and click Open.
              - The name of the cd should now appear next to the cd-rom
			    image in the Storage Tree panel.
         - Click OK to exit the Settings dialog.
    d. Make sure your virtual machine is hightlighted in the left panel of
	   VirtualBox Manager, and click the big green arrow at the top of the
	   the page. This will start your virtual machine, and the Ubuntu
	   installation process should commence.
	   
### Step 4: Install Ubuntu Server
    Click inside the window that displays your virtual machine's console, 
    and take your hands off your pointing device. From here, we are in
    character mode, and everything is done via the keyboard. If you use 
    your pointing device outside the virtual machine console, you will 
    have to click on the console again to resume the installation.
	
    The installation process is fairly straight-forward, but ...
      - Don't encrypt your home directory
      - Select 'Guided - use entire disk' on the Partition disks dialog
        (The disk section can be scarry, but remember, your virtual 
		machine sees a virtual disk ... which is a file on your computer)
      - Select Install security updates automatically
	  - IMPORTANT: When you get to Software Selection, pause ...
	      - Use the arrow keys to cursor down to 'LAMP server'
          - Then press the space bar to select 'LAMP server' and enter
          LAMP stands for Linux, Apache, MySQL, and PHP. Selecting this
          gives us more than we need, but simplifies the setup process.
      - Set the MySQL password for security, you can use the same
        password that you entered for your user password if you like.
      - Yes, Install the GRUB boot loader to the master boot record
      - You do not need to remove the ISO image at the end of the process,
        If you want to, just follow the process where you mounted the ISO.

### Step 5 Configure Ubuntu Server
    When installation is complete, you should be at the Ubuntu login
	screen. After you login, run the following commands:
      - sudo apt-get update
      - sudo apt-get install ssh
             allows you to use scp to transfer files to the server
	  - sudo mkdir /var/www/html/tzLib
             makes the folder for tzLib
      - sudo groupadd tzLib
             makes a user group for tzLib
      - sudo adduser <your user name> tzLib
             adds your user name to the tzLib group
      - sudo chgrp tzLib /var/www/html/tzLib
             adds the tzLib folder to the tzLib group
      - sudo chmod g+rwx /var/www/html/tzLib
             gives the tzLib group read/write/execute permissions to
             the tzLib folder.
      - sudo apt-get install php-pear
             adds software required to update the time-zone database
      - sudo apt-get install php-dev
             adds software required to update the time-zone database
      - sudo apt-get upgrade 
             applies upgrades (takes a while)
      - sudo pecl install timezonedb
             installs the latest time zone database from PHP
	Next, we edit a file with the nano editor		 
      - sudo nano /etc/php/7.0/apache2/php.ini
          - Scroll down until you see 'Dynamic Extensions' box as shown below
		  - Add the 'extension=timezonedb.so' statement as shown below
		  - Verify what you typed, and press ctrl-o
		  - Verify the file name, and press enter to save the file
		  - Press ctrl-x to exit the editor
		  
             ;;;;;;;;;;;;;;;;;;;;;;
             ; Dynamic Extensions ;
             ;;;;;;;;;;;;;;;;;;;;;;
			 
             extension=timezonedb.so

    That's it, you're done ... but before we reboot the server, you should
	capture the	server's IP address with the command
	  - ifconfig
          The IP address should be displayed on the second line, which 
		  starts with 'inet'. Please write the IP address down. You will
		  need it in the final step.

      - shutdown -r now
		  To reboot the server
			 
    You're done with the server console, so minimize it.
	

### Step 6: Place the PHP code on the server
	Two files  need to be transferred to the /var/www/html/tzLib
	folder that you created on the server. There are numerous ways to do
	this. This step will describe how to transfer the file with SCP.  If you 
	have a better way, go for it. 

    If you don't have an SCP client for your computer, find one and
	and install it. Then issue the command to transfer the files. The
    syntax will vary by OS, but should be similar to the authors commands
	which are detailed below.
	
	The author used pscp.exe for Windows which was downloaded from: 
	https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html
		 
    The SCP transfer process:
    - Open a Windows command line
    - Change directory to the folder containing GetJSON.php and GetTzDbVersion.php
    - Issue the following commands:
        pscp GetJSON.php <username>@<server_ipaddress>:/var/www/html/tzLib
		pscp GetDbVersion.php <username>@<server_ipaddress>:/var/www/html/tzLib
			
        The first time you use SCP with this host, you may be asked if you trust
        the host, or if you want to cache the host's key.
		
        You will then be prompted to enter your server password, 
		and the file will be transferred.

	To verify that every thing is working right,
    -	open a browser and enter: 
		http://<your browser's IP address>/tzLib/GetTzDbVersion.php
		This should display the version of the time zone database on your server. 
		The version should be 2017.2 or later.
	
    	
	  
	
		  
	  
	  
	  
	
	
	
    
	   
	
  
  
  
  
  
  