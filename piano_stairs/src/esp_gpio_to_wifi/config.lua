node.restore()
node.restart()
wifi.setmode(wifi.STATION)
station_cfg={}
station_cfg.ssid="PianoStairs"
station_cfg.pwd="StairsPiano"
station_cfg.save=true
station_cfg.auto=true
wifi.sta.config(station_cfg)

_, _, gw = wifi.sta.getip()

majorVer, minorVer, devVer, chipid, flashid, flashsize = node.info()
print("NodeMCU "..majorVer.."."..minorVer.."."..devVer)
print("Chip Id "..chipid.." Flash Id "..flashid.." Flash Size "..flashsize)
do
	local def_sta_config=wifi.sta.getdefaultconfig(true)
	print(string.format("\tDefault station config\n\tssid:\"%s\"\tpassword:\"%s\"%s", def_sta_config.ssid, def_sta_config.pwd, (type(def_sta_config.bssid)=="string" and "\tbssid:\""..def_sta_config.bssid.."\"" or "")))
end
