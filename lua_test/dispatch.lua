

local function _default_dispatch(msg, PTYPE)
    local p = protocol[PTYPE]
    if not p then
        error(string.format("handle unknown PTYPE: %s. sender %u", PTYPE, _decode(msg, "S")))
    end

    local sender, session, sz, len = _decode(msg, "SEC")
    if session > 0 then
        session_watcher[session] = nil
        local co = session_id_coroutine[session]
        if co then
            session_id_coroutine[session] = nil
            --print(coroutine.status(co))
            coresume(co, sz, len, PTYPE)
            --print(coroutine.status(co))
            return
        end

        if co ~= false then
            error(string.format("%s: response [%u] can not find co.", moon.name, session))
        end
    else
        local dispatch = p.dispatch
        if not dispatch then
            error(string.format("[%s] dispatch PTYPE [%u] is nil", moon.name, p.PTYPE))
            return
        end

        if not p.israw then
            local co = tremove(co_pool) or co_create(routine)
            if not p.unpack then
                error(string.format("PTYPE %s has no unpack function.", p.PTYPE))
            end
            coresume(co, dispatch, sender, session, p.unpack(sz, len))
        else
            dispatch(msg)
        end
    end
end

core.callback(_default_dispatch)