#!/bin/bash

##--------------------------------------------------------
 # Module Name : Bookmyname DNS updater
 # Version : 1.0.0
 #
 #
 # Copyright (c) 2015 zorglub42
 # This software is distributed under the Apache 2 license
 # <http://www.apache.org/licenses/LICENSE-2.0.html>
 #
 #--------------------------------------------------------
 # File Name   : updateDNS.sh
 #
 # Created     : 2015-07
 # Authors     : Zorglub42 <contact(at)zorglub42.fr>
 #
 # Description :
 #     Update your domain DNS configuration on bookmyname with your current public IP
 #--------------------------------------------------------
##
#bookmyname credentials
LOGIN=YOUR-BOOKMYNAME-LOGIN
PWD=YOUR-BOOKMYNAME-PASSWORD


#Internet domain to manage
DOMAIN=DOMAIN_TO_MANAGE #ex: DOMAIN=zorglub42.fr
#DNS configuration template location
DNS_RULES_FILE=$PWD/DNS.txt


#If required, the GW to use to detyermine public IP
INTERNET_GW=




CURRENT_IP_FILE=/var/run/current_public.ip



date
#Set some static routes to ensure that public IP is the one provided by ISP and not VPN
#ifconfig.me
if [ "$INTERNET_GW" != "" ] ; then
	#ifconfig.me
	/sbin/route del -host 49.212.202.172
	/sbin/route add -host 49.212.202.172 gw $INTERNET_GW
	#monip.org
	/sbin/route del -host 217.70.182.162
	/sbin/route add -host 217.70.182.162 gw $INTERNET_GW
	#bookmyname
	/sbin/route del -host 88.191.249.162
	/sbin/route add -host 88.191.249.162 gw $INTERNET_GW
	
	#icanhazip.com
	/sbin/route del -host 104.238.136.31
	/sbin/route add -host 104.238.136.31 gw $INTERNET_GW
fi


#Get Public IP
MY_IP=`curl -s http://icanhazip.com`
#MY_IP=`curl -s http://ifconfig.me/ip`
#MY_IP=`curl -s http://monip.org| grep "IP :"| awk '{print $5}'| awk -F'<' '{print $1}'`
CURRENT_IP="";
if [ -f $CURRENT_IP_FILE ] ; then
        CURRENT_IP=`cat $CURRENT_IP_FILE`
fi

if [ "$CURRENT_IP" == "$MY_IP" ] ; then
        echo "DNS is up to date";
        exit 0
fi


echo "Public IP has changed! Updating DNS"
echo $MY_IP > $CURRENT_IP_FILE

#Login
SID=`curl -s -i 'https://www.bookmyname.com/login.cgi' -H 'Content-Type: application/x-www-form-urlencoded' -d 'handle='$LOGIN'&passwd='$PWD'&url=%23URI%3B&submit.x=0&submit.y=0'| grep Set-Cookie|tail -1| awk '{print $2}'| awk -F';' '{print $1}'`
#Get DID (????)
DID=`curl -s -X POST 'https://www.bookmyname.com/manager.cgi?cmd=gdp'  -H "Cookie: $SID" -H 'Content-Type: application/x-www-form-urlencoded' --data 'domain='$DOMAIN'&submit.x=34&submit.y=15'| grep "\<input type=\"hidden\" name=\"did\" value="| awk -F'"' '{print $6}'`



#Generate DNS record POST param
DNS=""
cat $DNS_RULES_FILE|sed 's/ /+/g'|sed 's/$/%0D%0A/'|sed 's/@/%40/'|sed "s/PUBLIC_IP/$MY_IP/">/tmp/DNS.$$
while read l ; do
        DNS=`echo $DNS$l`
done</tmp/DNS.$$
rm /tmp/DNS.$$

#Update DNS
curl -s -i -X POST 'https://www.bookmyname.com/manager.cgi?cmd=gdp&mode=1' -H 'Referer: https://www.bookmyname.com/manager.cgi?cmd=gdp' -H "Cookie: $SID" -H 'Content-Type: application/x-www-form-urlencoded' --data 'gdp_zonefile='$DNS'&did='$DID'&mode=1&Submit=Valider'>/tmp/$$.res
grep "Erreur" /tmp/$$.res
grep "Votre demande de personnalisation" /tmp/$$.res
grep "quelques minutes" /tmp/$$.res
rm /tmp/$$.res

#Logout
curl 'https://www.bookmyname.com/logout.cgi' -H "Cookie: $SID"



