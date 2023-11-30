local co_pool = setmetatable({}, { __mode = "kv" })

local co_create = coroutine.create
local co_running = coroutine.running
local co_yield = coroutine.yield
local co_resume = coroutine.resume
local co_close = coroutine.close
local co_num = 0

local function invoke(co, fn, ...)
    fn(...)
    co_pool[#co_pool + 1] = co
    print("copool ", #co_pool)
end

local function routine(fn, ...)
    local co = co_running()
    invoke(co, fn, ...)
     while true do
        print("invoke yield")
         invoke(co, co_yield())
        print("invoke yield2")

     end
end


local tremove = table.remove
local co_create = coroutine.create
local coresume = coroutine.resume

---@param fn fun(...) @需要异步执行的函数
---@param ... any @可选参数，传递给 fn 函数
---@return thread @新创建的协程
function async(fn, ...)
    local co = table.remove(co_pool) or coroutine.create(routine)
    print("as", co)
    print("copool  async", #co_pool)
     
   coroutine.resume(co, fn, ...)
    return co
end
function quit()
    local running = co_running()
    print("quit", running)
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

--local coco  = async(add)
-- wakeup(coco)
-- async(add)
-- async(add)
-- print(#co_pool)

local w = async(function()
  local w = wait(200)
    print(w)
   -- print(co_running())
  -- local qq = wait(300)
   --print(qq)

end)


function wakeup(co, ...)
    local args = {...}
    coroutine.resume(co, false, "BREAK", args)
end

print(w)
-- local c = coroutine.resume(w)
print(coroutine.status(w))
-- print(c)
wakeup(w)
print(coroutine.status(w))

--wakeup(w)
print("new async")
local ww = async(function()
    local q = wait(300)
      print(q)
     -- print(co_running())
    -- local qq = wait(300)
     --print(qq)
     quit()
  end)
wakeup(ww)

quit()