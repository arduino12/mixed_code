LED_PIN=4
PINS={1,2,5,6,7,12}
SERVER_PORT=8888
SERVER_IP="10.0.0.1"

old_mask=255
udpSocket = net.createUDPSocket()
gpio.mode(LED_PIN,gpio.OUTPUT)

for i,pin in ipairs(PINS) do
	gpio.mode(pin,gpio.INPUT, gpio.PULLUP)
end

tmr.create():alarm(10,tmr.ALARM_AUTO,function()
	local mask=0
	for i,pin in ipairs(PINS) do
		if gpio.read(pin)==0 then
			mask=bit.set(mask,i-1)
		end
	end
	if old_mask~=mask then
		if wifi.sta.status()==wifi.STA_GOTIP then
			udpSocket:send(SERVER_PORT,SERVER_IP,mask)
		end
		gpio.write(LED_PIN,mask~=0 and gpio.LOW or gpio.HIGH)
		print(string.format("pins mask: %d",mask))
		old_mask=mask
	end
end)
