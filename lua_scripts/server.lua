
local core = require("core")

local uid = 0
local sessionid_coroutine = {}

local core_new_service = core.new_service
local server = core

local co_pool = setmetatable({}, { __mode = "kv" }) -- Э�̳�

------------------------------- ����ʼ -------------------------------

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
    local co = coroutine.running()
    sessionid_coroutine[uid] = co
    print("get sessionid:", uid, "co:", co)
    return uid
end

-- �����·���
function server.new_service(option)
    local sessionid = server.make_session()
    core_new_service(sessionid, option)
    print("new service3")
 --   return core.wait(sessionid)
end

-- �رյ�ǰ����
function server.quit()
    local cur_co = coroutine.running()
    -- �رճ���ǰЭ�̵ķ���
    for k, co in pairs(sessionid_coroutine) do
        if type(co) == "thread" and co ~= cur_co then
            coroutine.close(co)
            sessionid_coroutine[k] = false
        end
    end
    -- todo kill
end

------------------------------- ������� -------------------------------


------------------------------- ��Ϣ��ʼ -------------------------------

-- ��Ϣ����
function server.send()
    
end

-- ��Ϣת��
function server.dispatch()
    
end


------------------------------- ��Ϣ���� -------------------------------


print(package.path)
local op = { luafile="testservice.lua" }
server.new_service(op)
server.new_service(op)


return server