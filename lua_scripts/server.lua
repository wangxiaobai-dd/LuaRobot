
local core = require("core")

local uid = 0
local sessionid_coroutine = {}

local core_new_service = core.new_service
local server = core

local co_pool = setmetatable({}, { __mode = "kv" }) -- 协程池

------------------------------- 服务开始 -------------------------------

-- 协程封装
local function invoke(co, fn, ...)
    fn(...) -- 第一次匿名函数 yield
    co_pool[#co_pool + 1] = co
end

-- 协程封装
local function routine(fn, ...)
    local co = coroutine.running()
    invoke(co, fn, ...)
    while true do -- 为了被重复使用
        invoke(co, coroutine.yield()) -- yield时，不会进入invoke
    end
end

-- 异步接口
function server.async(fn, ...)
    -- 从现有协程池获取 或者 新创建
    local co = table.remove(co_pool) or coroutine.create(routine)
    coroutine.resume(co, fn, ...) -- 启动匿名函数
    return co
end

-- 映射协程id
function server.make_session()
    uid = uid + 1
    if nil ~= sessionid_coroutine[uid] then
        print("sessionid is used!")
    end
    local co = coroutine.running()
    sessionid_coroutine[uid] = co
    print("get sessionid:", uid, "co:", co)
    return uid
end

-- 创建新服务
function server.new_service(option)
    local sessionid = server.make_session()
    core_new_service(sessionid, option)
    print("new service3")
 --   return core.wait(sessionid)
end

-- 关闭当前服务
function server.quit()
    local cur_co = coroutine.running()
    -- 关闭除当前协程的服务
    for k, co in pairs(sessionid_coroutine) do
        if type(co) == "thread" and co ~= cur_co then
            coroutine.close(co)
            sessionid_coroutine[k] = false
        end
    end
    -- todo kill
end

------------------------------- 服务结束 -------------------------------


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