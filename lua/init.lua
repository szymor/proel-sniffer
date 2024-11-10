-- Tencent Cloud MQTT server in China
mqtt_server = "81.69.199.239"

uid = node.chipid()
clientid = "proelsniff"..uid
topic = "proelsniff/"..uid.."/"
uptime = 0

wifi.eventmon.register(wifi.eventmon.STA_GOT_IP, function(T)
  print("IP: "..T["IP"])
  print("Netmask: "..T["netmask"])
  print("Gateway: "..T["gateway"])
  m = mqtt.Client(clientid, 120)
  m:lwt(topic.."uptime", 0, 2, 0)
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
    m:publish(topic.."uptime", uptime, 2, 0)
  end
end)

print("MQTT server: "..mqtt_server)
print("Topic root: "..topic)

-- m:publish(topic.."flat", <flat_number_here>, 2, 0)
-- m:publish(topic.."idle_voltage", <idle_voltage_in_string_here>, 2, 0)
