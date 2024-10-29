mqtt_server = "81.69.199.239"

uid = node.chipid()
clientid = "proelsniff"..uid
topic = "proelsniff/"..uid.."/"
uptime = 0

wifi.eventmon.register(wifi.eventmon.STA_GOT_IP, function(T)
  m = mqtt.Client(clientid, 120)
  -- Tencent Cloud MQTT server in China
  print("Connecting to "..mqtt_server.."...")
  m:connect(mqtt_server)
  uptime = 0
end)

tmr.create():alarm(10000, tmr.ALARM_SINGLE, function()
  if wifi.sta.getip() == nil then
    print("Starting the Captive Portal...")
    enduser_setup.start()
  end
end)

tmr.create():alarm(10000, tmr.ALARM_AUTO, function()
  if m ~= nil then
    uptime = uptime + 10
	print("Heartbeat ("..uptime..").")
    m:publish(topic.."uptime", uptime, 0, 0)
  end
end)
