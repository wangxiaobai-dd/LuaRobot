require("service")
require("message")

local core = require("core")
local server = core



print(package.path)
local op = { luafile="testservice.lua" }
server.new_service(op)
server.new_service(op)

return server