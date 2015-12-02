Solution to add a start-stop push button to a RPI  
We're speaking here about a clean stop (init 0) and not a hard-reboot

INSTALL
---------
  * install wiringPI library http://wiringpi.com/download-and-install/
  * install start-stop-button daemon
     * git clone https://github.com/zorglub42/tools/
     * cd  tools/rpi/start-stop-button/
     * cp start-stop-button-daemon /usr/loca/bin
     * cp start-stop-button /etc/init.d
     * update-rc.d start-stop-button defaults
     * update-rc.d start-stop-button start
