
-- interrupt called when button is pressed
function btnINT(level)
	-- don't allow interrupt in interrupt
	if inInt then
		return
	else 
		inInt = true
	end
	-- 100ms debounce
	tmr.delay(100000)
	cu:send("1234")
	gpio.write(4, gpio.LOW)
	inInt = false
	end

-- function called when UDP packet received
function rxPkt(cu, c)
	if c == "1234" then
		print("correct response")
		gpio.write(4, gpio.HIGH)
	else
		print("incorrect response")
		end
	end

-- setup gpio pins
gpio.mode(1, gpio.INT, gpio.PULLUP)
gpio.trig(1, "down", btnINT)
gpio.mode(4, gpio.OUTPUT)
gpio.write(4, gpio.LOW)

-- setup UDP port
cu = net.createConnection(net.UDP)
cu:on("receive", rxPkt)
cu:connect(1234, "192.168.1.220")
inInt = false





wifi.setmode(wifi.STATION)
wifi.sta.config("PianoStairs","StairsPiano")
wifi.sta.connect()

LED_PIN=4
gpio.mode(LED_PIN,gpio.OUTPUT)

print(wifi.sta.getip())
x=1
udpSocket = net.createUDPSocket()
tmr.alarm(2,1000,1,function()
	udpSocket:send(8888,"192.168.1.220", "hi")
	x=x+1
end)
	print(x)
	if x>1000 then x=1 end
	p=tonumber(wifi.sta.status())
	print (p)
	if p==5 then
		gpio.write(LED_PIN, gpio.LOW)
		print("LED OFF")
	else
		gpio.write(LED_PIN, gpio.HIGH)
		print("LED ON")
	end
end)
udpSocket:send(1234,"192.168.1.220", "hefgdfgdfgfdgdfgdfgfdgdfgdfgllo")


udpSocket = net.createUDPSocket()
udpSocket:listen(8888)
udpSocket:on("receive", function(s, data, port, ip)
	print(string.format("received '%s' from %s:%d", data, ip, port))
	s:send(port, ip, "echo: " .. data)
end)
