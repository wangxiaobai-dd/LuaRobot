
local core = require("core")

local sessionid = 0
local sessionid_coroutine = {}

print("interface")

--- 给协程映射id
function make_session()
    sessionid = sessionid + 1
    if nil ~= sessionid_coroutine[sessionid] then
        print("sessionid is used!")
    end
    sessionid_coroutine[sessionid] = coroutine.running()
    return sessionid
end

function new_service(option)
    local sessionid = make_session()
    core.new_service(sessionid, option)
    print("new service")
 --   return core.wait(sessionid)
end

new_service()