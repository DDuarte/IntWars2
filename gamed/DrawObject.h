#pragma once
class DrawObject {
    public:
        virtual ~DrawObject() {};
        virtual void Draw() = 0;
        virtual void UnloadRessources() = 0;
        virtual void LoadRessources() = 0;
        void Reset() {
            UnloadRessources();
            LoadRessources();
        };
    protected:
        bool isLoaded = false;
};

