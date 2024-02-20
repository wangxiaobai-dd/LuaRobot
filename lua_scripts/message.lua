local core = require("core")
local service = require("service")
local server = core

Cmd = { byCmd = 0, byParam = 0 }


server.MSG_LUA = 1

------------------------------- 消息开始 -------------------------------

local protocol = {}

local function _default_dispatch(msg, type)
    local p = protocol[type]
    if not p then
        print("no dispatch type")
    end

    local session = 0
    if session > 0 then
        local co = service.session_id_coroutine[session]
        if co then
            service.session_id_coroutine[session] = nil
            coroutine.resume(co, type)
            return
        end
    else
        local dispatch = p.dispatch
        if not dispatch then
            print("dispatch is null")
            return
        end

        dispatch(msg)
    end
end

core.callback(_default_dispatch)


function server.register(p)
    protocol[p.type] = p
end

local reg_protocol = server.register

reg_protocol {
    type = server.MSG_LUA,
    pack = server.pack,
    unpack = server.unpack,
    dispatch = function()
        error("PTYPE_LUA dispatch not implemented")
    end
}

-- 消息发送
function server.send(type, receiver, ...)
    local p = protocol[type]
    if not p then
        print("unknown send type:%u", type)
    end
    core.send(receiver, p.pack(...))
end

-- 消息转发
function server.dispatch()

end

function sendMsgTest()
    local send = Cmd()
end

function handleOne(msg)
    print "one one"
end

local handleMsg = {
    [1] = handleOne()
}


------------------------------- 消息结束 -------------------------------
