
local core = require("core")

local uid = 0
local sessionid_coroutine = {}
local core_new_service = core.new_service
local server = core


local co_num = 0
local co_pool = setmetatable({}, { __mode = "kv" })

local function invoke(co, fn, ...)
    co_num = co_num + 1
    fn(...)
    co_num = co_num - 1
    co_pool[#co_pool + 1] = co
end

local function routine(fn, ...)
    local co = co_running()
    invoke(co, fn, ...)
    while true do
        invoke(co, co_yield())
    end
end

local function coresume(co, ...)
    local ok, err = co_resume(co, ...)
    if not ok then
        err = traceback(co, tostring(err))
        co_close(co)
        error(err)
    end
    return ok, err
end

-- 异步接口
function server.async(fn, ...)
    local co = table.remove(co_pool) or coroutine.create(routine)
    coresume(co, fn, ...)
    return co
end

-- 映射协程id
function server.make_session()
    uid = uid + 1
    if nil ~= sessionid_coroutine[uid] then
        print("sessionid is used!")
    end
    sessionid_coroutine[uid] = coroutine.running()
    print("get sessionid:"..uid)
    return uid
end

-- 创建新服务
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