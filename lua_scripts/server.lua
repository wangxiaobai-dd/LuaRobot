
require("core")
local server = require("server")

local uid = 0
local sessionid_coroutine = {}

print("interface")

--- 给协程映射id
function make_session()
    uid = uid + 1
    if nil ~= sessionid_coroutine[uid] then
        print("sessionid is used!")
    end
    sessionid_coroutine[uid] = coroutine.running()
    print("get sessionid:"..uid)
    return uid
end

function new_service(option)
    local sessionid = make_session()
    server.new_service(sessionid, option)
    print("new service3")
 --   return core.wait(sessionid)
end

print(package.path)
local op = { luafile="testservice.lua" }
new_service(op)