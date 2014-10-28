---
--llyLuaBase.lua
--lua基础函数，系统所用
--某些函数可在release时注销其内容
--卢乐颜
--2014.11.2

--命名空间
lly = lly or {}

--用于main函数，进行错误输出，用法如下
--local status, msg = xpcall(main, lly.traceback)
--if not status then error(msg) end
--最终release时，可把此函数内容注释掉
function lly.traceback(msg)
	---[[
	lly.log("----------------------------------------")
	lly.log("LUA ERROR: " .. tostring(msg) .. "\n")
	lly.log(debug.traceback())
	lly.log("----------------------------------------")
	--]]
	return msg
end



---
--自定义的log类，便于修改和调试

-- cclog
--最终release时，可把此函数内容注释掉
function lly.log(...)
	---[[
	print(string.format(...))
	--]]
end

--自定义的log输出，调试时，输出时同时打印位置，所在函数名，以及所在文件名
--最终release时，可把此函数内容注释掉
function lly.logCurLocAnd(...)
	---[[
	local info = debug.getinfo(2,"Sln")
	local strInfo = "(^_^)/: " .. string.format(...) .. " @ LINE " .. info.currentline .. " IN " .. 
		(info.name and ("FUCN: " .. info.name .. " << ") or "FUCN: unnamed << ") .. info.short_src

	lly.log(strInfo)
	--]]
end

--打印当前位置函数的调用追溯
function lly.logTraceback()
	---[[
	lly.log("(O_O)/ this func is called from :")
	for nLevel = 3, math.huge do
		local info = debug.getinfo(nLevel,"Sln")
		if not info then break end
		local strInfo = " " .. info.short_src .. ">> LINE: " .. info.currentline ..
			(info.name and (" FUCN: " .. info.name .. " ") or " FUCN: unnamed ")
		lly.log(strInfo)
	end
	--]]
end



---
--禁止产生全局变量的函数，定义在主函数中，此后将不可新建全局变量，避免因写错名称但无法检测而造成的麻烦

--禁止所有的全局变量
function lly.finalizeGlobalEnvironment()
	local mt = {}	
	
	mt.__index = function (t, k)
		error("(>_<)/undeclared var : " .. k, 2)
	end

	mt.__newindex = function (t, k, v)
		error("(>_<)/undeclared var : " .. k, 2)
	end

	setmetatable(_G, mt)
end

--只禁止当前文件的全局变量，会改变当前文件的环境
function lly.finalizeCurrentEnvironment()
	local mt = {}
	
	mt.__index = function (t, k)
		local globalValue = _G[k]
		
		if globalValue == nil then
			error("(>_<)/undeclared var : " .. k, 2)
		else
			return globalValue
		end
	end

	mt.__newindex = function (t, k, v)
		error("(>_<)/undeclared var : " .. k, 2)
	end
	
	local newgt = {}
	setmetatable(newgt,mt)
	
	setfenv(2, newgt)
end



---
--使类的实例最终化，也就是这之后不能再添加新的属性，避免因写错名称但无法检测而造成的麻烦
--适用于
--	自定义的c class、
--	自定义的lua class
--	自定义的结构
--最终release时，可把此函数内容注释掉
local mt_table = {} --保存原有的mt的index函数

function lly.finalizeInstance(ins)
	---[[
	local mt = getmetatable(ins)

	if mt ~= nil then 
		--有元表
		local strClassName = ins.__cname --对象的类名

		if mt_table[strClassName] == nil then
			mt_table[strClassName] = {}
			mt_table[strClassName].__index = mt.__index
			mt_table[strClassName].__newindex = mt.__newindex
		end

		mt.__index = function (t, k)
			local __idxTmp = mt_table[strClassName].__index

			local result = nil
			if type(__idxTmp) == "function" then
				result = __idxTmp(t, k)
			else
				result = __idxTmp[k]
			end

			if result == nil then
				error("(>_<)/no this attribute : " .. k, 2)
			end
			return result
		end

		mt.__newindex = function (t, k, v)
			local __idxTmp = mt_table[strClassName].__index

			local result = nil
			if type(__idxTmp) == "function" then
				result = __idxTmp(t, k)
			else
				result = __idxTmp[k]
			end
			
			if result == nil then
				error("(>_<)/no this attribute : " .. k, 2)
			else
				local __nwidxTmp = mt_table[strClassName].__newindex
				if type(__nwidxTmp) == "function" then
					__nwidxTmp(t, k, v)
				else
					__nwidxTmp[k] = v
				end
			end
		end	
	else
		mt = {}	
		
		mt.__index = function (t, k)
			error("(>_<)/no this attribute " .. k, 2)
		end

		mt.__newindex = function (t, k, v)
			error("(>_<)/no this attribute " .. k, 2)
		end

		setmetatable(ins, mt)			
	end
	--]]
end



---
--创建类的函数，基本等于cocos3.2的extern。lua中的class函数，
--但里面为所创建的类增加了必须实现的函数，包括：
--	ctor 用于声明所有的常量，属性，以及函数（函数声明为function () end）
--	implementFunction 用于实现在ctor中声明的函数
--						因为在ctor之后会用finalizeInstance使得类不
--						可再增加新的属性，所以在此实现是比较不易出错的
--	init 用于初始化，调用create时调用
--还有一个create作为模板，生成对象用，create可以自行修改或者禁用

--克隆，完全复制于extern
function lly.clone(object)
	local lookup_table = {}
	local function _copy(object)
		if type(object) ~= "table" then
			return object
		elseif lookup_table[object] then
			return lookup_table[object]
		end
		local new_table = {}
		lookup_table[object] = new_table
		for key, value in pairs(object) do
			new_table[_copy(key)] = _copy(value)
		end
		return setmetatable(new_table, getmetatable(object))
	end
	return _copy(object)
end

--Create an class.
function lly.class(classname, super)
	local superType = type(super)
	local cls

	if superType ~= "function" and superType ~= "table" then
		superType = nil
		super = nil
	end
	
	--如果从userdata，或继承于userdata的table中继承c的类
	if superType == "function" or (super and super.__ctype == 1) then
		-- inherited from native C++ Object
		cls = {}

		if superType == "table" then
			-- copy fields from super
			for k,v in pairs(super) do cls[k] = v end
			cls.__create = super.__create
			cls.super    = super
		else
			cls.__create = super
		end

		cls.ctor = function(...) error("need implement", 2) end--必须重载
		cls.__cname = classname
		cls.__ctype = 1

		function cls.new(...)
			local instance = cls.__create(...)
			-- copy fields from class to native object
			for k,v in pairs(cls) do instance[k] = v end
			instance.class = cls
			instance:ctor(...)
			return instance
		end

	else
		-- inherited from Lua Object
		if super then
			cls = clone(super)
			cls.super = super
		else
			cls = {ctor = function(...) error("need ctor", 2) end}--必须重载
		end

		cls.__cname = classname
		cls.__ctype = 2 -- lua
		cls.__index = cls

		function cls.new(...)
			local instance = setmetatable({}, cls)
			instance.class = cls
			instance:ctor(...)
			return instance
		end
	end
	
	--自添加
	--实现ctor中的函数
	function cls:implementFunction() error("need implement func", 2) end

	--初始化
	function cls:init(...) error("need init", 2) end

	--工厂函数，创建对象
	function cls:create(...)--返回class的对象
		local pRet = self.new()
		lly.finalizeInstance(pRet)--最终化对象
		pRet:implementFunction()
		b = pRet:init(...)  
		if b then
			return pRet
		else
			lly.log("(>_<)/init return false")
			pRet = nil
			return nil
		end
	end

	return cls
end

--创建一个结构体，基本为 一个简化的创建类的方法
--在函数的ctor方法要返回 一个结构体
--创建好以后，用create生成的对象，不能再往里面添加内容
function lly.struct(create_table_func)	
	local stru = {}
	stru.table_ctor = create_table_func	

	--工厂函数，创建对象
	function stru:create()--返回struct的对象
		local pRet = self.table_ctor()
		if type(pRet) == "table" then
			lly.finalizeInstance(pRet)--最终化对象
			return pRet
		end
	end

	return stru
end

--



