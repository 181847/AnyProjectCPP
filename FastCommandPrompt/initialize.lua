print("prepare lua")
package.path = package.path..";../?.lua"
package.cpath = package.cpath..";../?.dll"

controllerName = "controller"
controllerFilePath = package.searchpath(controllerName, package.path)
print("controller has found "..controllerFilePath)

clipboard = require("clipboard")

function GetNormalComment()
	date = os.date('%Y/%m/%d')
	user = 'liuyang28'
	comment = '/**\r\n* ['..date..' '..user..'] \r\n*/';
	return comment;
end

function CallController()
	controller = assert(loadfile(controllerFilePath), "controller not exist.")
	controller()
end

print('initialize script complete')
