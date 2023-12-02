
local core = require("core")

local uid = 0
local sessionid_coroutine = {}
local core_new_service = core.new_service
local server = core

local co_pool = setmetatable({}, { __mode = "kv" }) -- Э�̳�

-- Э�̷�װ
local function invoke(co, fn, ...)
    fn(...) -- ��һ���������� yield
    co_pool[#co_pool + 1] = co
end

-- Э�̷�װ
local function routine(fn, ...)
    local co = coroutine.running()
    invoke(co, fn, ...)
    while true do -- Ϊ�˱��ظ�ʹ��
        invoke(co, coroutine.yield()) -- yieldʱ���������invoke
    end
end

-- �첽�ӿ�
function server.async(fn, ...)
    -- ������Э�̳ػ�ȡ ���� �´���
    local co = table.remove(co_pool) or coroutine.create(routine)
    coroutine.resume(co, fn, ...) -- ������������
    return co
end

-- ӳ��Э��id
function server.make_session()
    uid = uid + 1
    if nil ~= sessionid_coroutine[uid] then
        print("sessionid is used!")
    end
    sessionid_coroutine[uid] = coroutine.running()
    print("get sessionid:", uid, "co:", coroutine.running())
    return uid
end

-- �����·���
function server.new_service(option)
    local sessionid = server.make_session()
    core_new_service(sessionid, option)
    print("new service3")
 --   return core.wait(sessionid)
end

-- ʹ��ǰЭ�̷����˳�
function server.quit()
    local co = coroutine.running()
    
end

print(package.path)
local op = { luafile="testservice.lua" }
server.new_service(op)

return server