
local core = require("core")

local uid = 0
local sessionid_coroutine = {}
local core_new_service = core.new_service
local server = core


local co_pool = setmetatable({}, { __mode = "kv" })

-- 协程封装
local function invoke(co, fn, ...)
    fn(...)
    co_pool[#co_pool + 1] = co
    print("copool ", #co_pool)
end

-- 协程封装
local function routine(fn, ...)
    local co = coroutine.running()
    invoke(co, fn, ...)
    while true do -- 为了被重复使用
        invoke(co, coroutine.yield())
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
    -- 从现有协程池获取 或者 新创建
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

-- 使当前协程服务退出
function server.quit()
    local co = coroutine.running()
    
end

print(package.path)
local op = { luafile="testservice.lua" }
server.new_service(op)

return server