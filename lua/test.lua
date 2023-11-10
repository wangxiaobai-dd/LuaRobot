local co_pool = setmetatable({}, { __mode = "kv" })

local co_running = coroutine.running
local co_yield = coroutine.yield
local co_num = 0

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

local tremove = table.remove
local co_create = coroutine.create
local coresume = coroutine.resume

---@param fn fun(...) @需要异步执行的函数
---@param ... any @可选参数，传递给 fn 函数
---@return thread @新创建的协程
function async(fn, ...)
    local co = tremove(co_pool) or co_create(routine)
    coresume(co, fn, ...)
    return co
end

local co_yield = coroutine.yield

function wait(sessionid)
    local a = coroutine.yield()

    if a then
        return 100
    else
        return sessionid
    end
end

function add()
    print(1)
end

local coco  = async(add)
-- wakeup(coco)
-- async(add)
-- async(add)
-- print(#co_pool)

local w = async(function()
    local w = wait(200)
    print(w)
    print(co_running())
end)


function wakeup(co, ...)
    local args = {...}
    coroutine.resume(co, true, "BREAK", args)
end

print(w)
-- local c = coroutine.resume(w)
-- print(c)
wakeup(w)