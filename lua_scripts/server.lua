
local core = require("core")

local uid = 0
local sessionid_coroutine = {}
local core_new_service = core.new_service
local server = core

function server.async(fn, ...)
    local co = tremove(co_pool) or co_create(routine)
    coresume(co, fn, ...)
    return co
end

--- ∏¯–≠≥Ã”≥…‰id
function server.make_session()
    uid = uid + 1
    if nil ~= sessionid_coroutine[uid] then
        print("sessionid is used!")
    end
    sessionid_coroutine[uid] = coroutine.running()
    print("get sessionid:"..uid)
    return uid
end

function server.new_service(option)
    local sessionid = server.make_session()
    core_new_service(sessionid, option)
    print("new service3")
 --   return core.wait(sessionid)
end

print(package.path)
local op = { luafile="testservice.lua" }
server.new_service(op)

return server