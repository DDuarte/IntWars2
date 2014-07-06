#ifndef LUAVM_H_
#define LUAVM_H_
#define ENABLE_LUA
using namespace luabind;

class CLuaVM {
    public:
        void register_modules();

        CLuaVM();
        ~CLuaVM();
        void Collect();
        void NewVM();
        void ReleaseVM();
        template<class T>
        void setGlobal(const char *szGlobal, T obj);
        luabind::object getFunction(luabind::object env, const char *szFunction);
        BOOL ExecuteFunction(const char *szFunction);
        void callFunction(const char *szFunction);
        /*void callFunction(const char *szFunction, CLoLPacket *p) {
            for(UINT i = 0; i < vEnvironments.size(); i++) {
                object fnc = getFunction(vEnvironments[i], szFunction);
                if(fnc) {
                    try {
                        call_function<void>(fnc, p);
                    } catch(error &e) {
                        const char *szError = lua_tostring(L, -1);
                        //LPrintChat(szError);
                    }
                }
            }
        }*/
        template <class T>
        void callFunction(const char *szFunction, T t);
        template <class T1, class T2>
        UINT callFunction(const char *szFunction, T1 t1, T2 t2);
        luabind::object share_table;
        lua_State *L;
        BOOL bLock;
        BOOL bLoaded;
        bool bRunning;
        std::vector<luabind::object> vEnvironments;
        //BOOL LoadFile(char *szFileName);
        //BOOL LoadScript(char *szName, char *szScript, UINT len, UINT ID = -1);
        luabind::object GetFnc(char *szPath, char *szFnc);
        template <class T1, class T2, class T3, class T4>
        UINT RunScript(char *szPath, char *szFnc, T1 t1, T2 t2, T3 t3, T4 t4)  {
            if(bRunning) {
                return 0;
            }
            bRunning = true;
            int r = 0;
            #ifdef ENABLE_LUA
            luabind::object fnc = GetFnc(szPath, szFnc);
            if(fnc && luabind::type(fnc) == LUA_TFUNCTION) {
                try {
                    call_function<void>(fnc, t1, t2, t3, t4);
                } catch(error &e) {
                    const char *szError = lua_tostring(L, -1);
                    printf("Error: %s\r\n", szError);
                }
            }
            #endif
            bRunning = false;
            return r;
        }
        void UnloadScripts();
};

extern CLuaVM *luaVM;

extern std::string base64_encode(unsigned char const *bytes_to_encode, unsigned int in_len);
extern void LUpdateWindow();
#endif