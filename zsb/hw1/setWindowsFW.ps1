# In our scenario ....
# we are a end workstation in a small comapny.
# our workstation is connected to web (front-end) server.
# our workstation sends requests to mail server.
# our workstation cannot communicate with other devices both outside and inside.


# VARIABLES
$MAIL_SERVER_ADDRESS = "10.0.0.2"
$MAIL_SERVER_PORT = "587"
$FRONT_WEB_SERVER_ADDRESS = "10.0.0.3"
$FRONT_WEB_SERVER_PORT = "443"


# enable Windows Firewall
Set-NetFirewallProfile -Profile Domain,Public,Private -Enabled True -ErrorAction Stop
# set default rules - white-lists for both inbound and outbound communication
Set-NetFirewallProfile -DefaultInboundAction Block -DefaultOutboundAction Block –NotifyOnListen True -AllowUnicastResponseToMulticast True –LogFileName %SystemRoot%\System32\LogFiles\Firewall\pfirewall.log

# set rules
# allow requests to mail server
New-NetFirewallRule -DisplayName "Allow mail server inbound" -Direction Inbound -RemoteAddress "$MAIL_SERVER_ADDRESS" -RemotePort "$MAIL_SERVER_PORT" -Action Allow
# allow communication with front-end web server
New-NetFirewallRule -DisplayName "Allow web front-end server inbound" -Direction Inbound -RemoteAddress "$FRONT_WEB_SERVER_ADDRESS" -RemotePort "$FRONT_WEB_SERVER_PORT" -Action Allow
New-NetFirewallRule -DisplayName "Allow web front-end server outbound" -Direction outbound -RemoteAddress "$FRONT_WEB_SERVER_ADDRESS" -RemotePort "$FRONT_WEB_SERVER_PORT" -Action Allow