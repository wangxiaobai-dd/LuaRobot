require("service")

local core = require("core")
local server = core

------------------------------- 消息开始 -------------------------------

-- 消息发送
function server.send()
    
end

-- 消息转发
function server.dispatch()
    
end


------------------------------- 消息结束 -------------------------------


print(package.path)
local op = { luafile="testservice.lua" }
server.new_service(op)
server.new_service(op)

return server