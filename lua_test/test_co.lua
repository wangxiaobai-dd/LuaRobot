local co_pool = setmetatable({}, { __mode = "kv" })

local co_create = coroutine.create
local co_running = coroutine.running
local co_yield = coroutine.yield
local co_resume = coroutine.resume
local co_close = coroutine.close
local co_num = 0

local co_id = {}

local function invoke(co, fn, ...)
    print("invoke")
  --  print(fn)
    fn(...) -- 匿名函数中yield 
    co_pool[#co_pool + 1] = co
    co_id[co] = #co_pool

    print("invoke finish ", #co_pool)
end

local function routine(fn, ...)
    local co = co_running()
  --   fn(...)
    -- co_pool[#co_pool + 1] = co
 invoke(co, fn, ...) -- 匿名函数中 wait 
     while true do
        print("invoke yield")
      --  co_yield() -- wakeup
    --    co_pool[#co_pool + 1] = co
       invoke(co, co_yield()) -- 不会进入invoke 
        print("invoke yield2")

     end
end


local tremove = table.remove
local co_create = coroutine.create
local coresume = coroutine.resume


function async(fn, ...)
    local co = table.remove(co_pool) or coroutine.create(routine)
     print("as", co)     
  coroutine.resume(co, fn, ...)
    return co
end

function quit()
    local running = co_running()
    print("quit", running)
end

local co_yield = coroutine.yield

function wait(sessionid)
    print("wait")
    local a = coroutine.yield()

    if a then
        return 100
    else
        return sessionid
    end
end

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

-- local c = coroutine.resume(w)
print("status1",coroutine.status(w))
-- print(c)
wakeup(w)
print("status2",coroutine.status(w))

local www = async(function()
    local ww = wait(300)
      print(ww)

  end)
  --wakeup(www)