require("service")

local core = require("core")
local server = core

------------------------------- ��Ϣ��ʼ -------------------------------

-- ��Ϣ����
function server.send()
    
end

-- ��Ϣת��
function server.dispatch()
    
end


------------------------------- ��Ϣ���� -------------------------------


print(package.path)
local op = { luafile="testservice.lua" }
server.new_service(op)
server.new_service(op)

return server