-- Parse command-line arguments
local args = {...}
if #args < 6 or args[1] ~= "-p" or args[3] ~= "-k" or args[5] ~= "-m" then
    io.stderr:write("Usage: lua mimic.lua -p dbPath -k key -m searchMode\n")
    os.exit(1)
end

local dbPath = args[2]
local key = args[4]
local searchMode = args[6]

if not (searchMode == "memory" or searchMode == "btree") then
    io.stderr:write("Invalid search mode. It should be either 'memory' or 'btree'.\n")
    os.exit(1)
end

local ffi = require("ffi")

ffi.cdef[[
    typedef struct {
        int clientId;
        int expirationDate;
        int randomSize;
    } DecryptedBlock;
    typedef struct {
        int version;
        int clientId;
        int encryptedBlockSize;
        DecryptedBlock decryptedBlock;
    } HyperHeaderBlock;
    typedef enum {
        MEMORY,
        BTREE
    } SearchType;

    typedef struct {
        int headerLength;
        int* HeaderPtr;
        char** HeaderSip;
    } BtreeModeParam;

    typedef struct {
        void* file;
        BtreeModeParam* btreeModeParam;
        SearchType searchType; // 使用枚举类型来定义searchType字段
        HyperHeaderBlock* hyperHeaderBlock;
        int startIndexPtr;
        int endIndexPtr;
        char* geoMapData;
        int ipType;
        int ipBytesLength;
        int columnSelection;
        char* dbBin;
        int indexLength;
    } DBSearcher;

    DBSearcher* initDBSearcher(char* dbFilePath, char* key, SearchType searchType);
    int search(char* ipString, DBSearcher* dbSearcher, char* region, int regionLen);
    void closeDBSearcher(DBSearcher* dbSearcher);
    void info(DBSearcher* dbSearcher);
]]

local lib = ffi.load("./tmp/libdb_searcher.dylib")  -- Adjust path as necessary

-- Initialize DBSearcher
local mode = (searchMode == "memory") and 0 or 1  -- Assuming MEMORY=0 and BTREE=1

local c_dbPath = ffi.new("char[?]", #dbPath + 1)
ffi.copy(c_dbPath, dbPath)

local c_key = ffi.new("char[?]", #key + 1)
ffi.copy(c_key, key)

local dbSearcher = lib.initDBSearcher(c_dbPath, c_key, mode)
if dbSearcher == nil then
    io.stderr:write("Failed to initialize DBSearcher.\n")
    os.exit(1)
end

-- Function to remove trailing newline
local function trimNewline(s)
    return s:gsub("\n$", "")
end

-- Interactive query loop
while true do
    io.write("Enter IP address (or 'q' to quit): ")
    local ip = io.read()
    ip = trimNewline(ip)

    if ip == "q" then
        break
    end

    local c_ip = ffi.new("char[?]", #ip + 1)
    ffi.copy(c_ip, ip)

    local region = ffi.new("char[?]", 100)
    if lib.search(c_ip, dbSearcher, region, 100) == 0 then
        print("Region: " .. ffi.string(region))
    else
        print("IP not found or an error occurred.")
    end
end

-- Cleanup
lib.closeDBSearcher(dbSearcher)